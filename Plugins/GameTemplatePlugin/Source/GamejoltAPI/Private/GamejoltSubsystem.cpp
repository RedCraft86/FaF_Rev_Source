// Copyright (C) RedCraft86. All Rights Reserved.

#include "GamejoltSubsystem.h"
#include "Interfaces/IHttpResponse.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Compression/OodleDataCompressionUtil.h"
#include "Serialization/BufferArchive.h"
#include "Helpers/AESHelpers.h"
#include "GTConfigSubsystem.h"
#include "GamejoltSettings.h"
#include "GamejoltKeys.h"
#include "GamejoltAPI.h"
#include "HttpModule.h"

#define CALLBACK(Return, ...) { if (Callback) { Callback(__VA_ARGS__); } return Return; }
#define MAKE_REQUEST(BaseURL, bUser, ...) const FString URL(ConstructURL(BaseURL, bUser)); CreateRequest(URL, __VA_ARGS__)

UGamejoltSubsystem::UGamejoltSubsystem()
{
	GameData.Key = 0;
	GameData.Value = TEXT("");
	Credentials.Key = TEXT("");
	Credentials.Value = TEXT("");
	TestConnectionData.Key = false;
	TestConnectionData.Value = 0;
}

void UGamejoltSubsystem::TestConnection(const bool bAutoLogin)
{
	TestConnectionData.Value = 1;
	TestConnectionData.Key = bAutoLogin;
	GetDataKeys(false, [this](const FGamejoltResponse& Response, const TArray<FString>&)
	{
		if (Response.bSuccess)
		{
			TestConnectionData.Value = 0;
			OnConnectionSuccessful.Broadcast();
			if (TestConnectionData.Key)
			{
				LoadCredentials([this](const FGamejoltResponse& InResponse)
				{
					OnAutoLoginUpdate.Broadcast(InResponse.bSuccess);
				});
			}
		}
		else
		{
			if (TestConnectionData.Value <= 5)
			{
				TestConnectionData.Value++;
				TestConnection(TestConnectionData.Key);
			}
			else
			{
				TestConnectionData.Value = -1;
				OnConnectionTimedOut.Broadcast();
				UE_LOG(GamejoltAPI, Warning, TEXT("Connection Timed Out!"))
			}
		}
	});
}

void UGamejoltSubsystem::GetUserData(TFunction<void(const FGamejoltResponse&, const FGamejoltUserData&)> Callback) const
{
	if (!IsLoggedIn())
	{
		CALLBACK(, GamejoltResponse::NotLoggedIn, {});
	}

	MAKE_REQUEST(TEXT("users/?"), true, [Callback](const FGamejoltResponse& Response)
	{
		FGamejoltUserData OutData;
		if (Response.bSuccess)
		{
			const TArray<TSharedPtr<FJsonValue>>* DataArray;
			Response.Json.JsonObject->TryGetArrayField(TEXT("users"), DataArray);
			if (DataArray && !DataArray->IsEmpty() && (*DataArray)[0].IsValid())
			{
				TSharedPtr<FJsonObject>* DataObject;
				(*DataArray)[0]->TryGetObject(DataObject);
				if (DataObject && DataObject->IsValid())
				{
					int32 SignedUp, LoggedIn = 0;
					const TSharedPtr<FJsonObject> DerefObj = *DataObject;
					DerefObj->TryGetNumberField(TEXT("id"), OutData.ID);
					DerefObj->TryGetStringField(TEXT("type"), OutData.Type);
					DerefObj->TryGetStringField(TEXT("username"), OutData.UserName);
					DerefObj->TryGetStringField(TEXT("avatar_url"), OutData.AvatarURL);
					DerefObj->TryGetStringField(TEXT("signed_up"), OutData.SignedUpRelativeTime);
					DerefObj->TryGetNumberField(TEXT("signed_up_timestamp"), SignedUp);
					DerefObj->TryGetStringField(TEXT("last_logged_in"), OutData.LoggedInRelativeTime);
					DerefObj->TryGetNumberField(TEXT("last_logged_in_timestamp"), LoggedIn);
					DerefObj->TryGetStringField(TEXT("status"), OutData.Status);
					DerefObj->TryGetStringField(TEXT("developer_name"), OutData.DeveloperName);
					DerefObj->TryGetStringField(TEXT("developer_website"), OutData.DevWebsiteURL);
					DerefObj->TryGetStringField(TEXT("developer_description"), OutData.DevDescription);
					OutData.SetTimestamps(SignedUp, LoggedIn);
				}
			}
		}

		CALLBACK(, Response, OutData);
	});
}

void UGamejoltSubsystem::AuthLogin(const FString& UserName, const FString& UserToken, const bool bRemember, TFunction<void(const FGamejoltResponse&)> Callback)
{
	if (UserName.IsEmpty() || UserToken.IsEmpty()) CALLBACK(, {false, TEXT("No credentials provided")})
	if (UserName == Credentials.Key && UserToken == Credentials.Value) CALLBACK(, {true, TEXT("")})
	if (IsLoggedIn()) Logout(bRemember);
	
	Credentials.Key = UserName;
	Credentials.Value = UserToken;
	MAKE_REQUEST(TEXT("users/auth/?"), true, [this, bRemember, Callback](const FGamejoltResponse& Response)
	{
		if (!Response.bSuccess)
		{
			Credentials.Key = TEXT("");
			Credentials.Value = TEXT("");
		}
		else if (bRemember)
		{
			SaveCredentials();
		}

		CALLBACK(, Response);
	});
}

void UGamejoltSubsystem::Logout(const bool bForget)
{
	Credentials.Key = TEXT("");
	Credentials.Value = TEXT("");
	if (bForget) DeleteCredentials();
}

bool UGamejoltSubsystem::IsLoggedIn() const
{
	return !Credentials.Key.IsEmpty() && !Credentials.Value.IsEmpty();
}

void UGamejoltSubsystem::DeleteData(const FString& Key, const bool bUser, TFunction<void(const FGamejoltResponse&)> Callback) const
{
	if (Key.IsEmpty()) CALLBACK(, GamejoltResponse::InvalidParams);
	if (!IsLoggedIn() && bUser) CALLBACK(, GamejoltResponse::NotLoggedIn)
	MAKE_REQUEST(FString::Printf(TEXT("data-store/remove/?&key=%s"), *FGenericPlatformHttp::UrlEncode(Key)), bUser,
		[Callback](const FGamejoltResponse& Response)
		{
			CALLBACK(, Response);
		});
}

void UGamejoltSubsystem::SetData(const FString& Key, const FString& Value, const bool bUser, TFunction<void(const FGamejoltResponse&)> Callback) const
{
	if (Key.IsEmpty()) CALLBACK(, GamejoltResponse::InvalidParams);
	if (!IsLoggedIn() && bUser) CALLBACK(, GamejoltResponse::NotLoggedIn)
	
	if (Value.IsEmpty()) DeleteData(Key, bUser, Callback);
	MAKE_REQUEST(FString::Printf(TEXT("data-store/set/?&key=%s&data=%s"),
		*FGenericPlatformHttp::UrlEncode(Key), *FGenericPlatformHttp::UrlEncode(Value)), bUser,
		[Callback](const FGamejoltResponse& Response)
		{
			CALLBACK(, Response);
		});
}

void UGamejoltSubsystem::GetData(const FString& Key, const bool bUser, TFunction<void(const FGamejoltResponse&, const FString&)> Callback) const
{
	if (Key.IsEmpty()) CALLBACK(, GamejoltResponse::InvalidParams, TEXT(""));
	if (!IsLoggedIn() && bUser) CALLBACK(, GamejoltResponse::NotLoggedIn, TEXT(""))
	MAKE_REQUEST(FString::Printf(TEXT("data-store/?&key=%s"), *FGenericPlatformHttp::UrlEncode(Key)), bUser,
		[Callback](const FGamejoltResponse& Response)
		{
			FString Data = TEXT("");
			Response.Json.JsonObject->TryGetStringField(TEXT("data"), Data);
			CALLBACK(, Response, Data);
		});
}

void UGamejoltSubsystem::GetDataKeys(const bool bUser, TFunction<void(const FGamejoltResponse&, const TArray<FString>&)> Callback) const
{
	if (!IsLoggedIn() && bUser) CALLBACK(, GamejoltResponse::NotLoggedIn, {})
	MAKE_REQUEST(TEXT("data-store/get-keys/?"), bUser, [Callback](const FGamejoltResponse& Response)
	{
		TArray<FString> Keys;
		if (Response.bSuccess)
		{
			const TArray<TSharedPtr<FJsonValue>>* KeyValues;
			if (Response.Json.JsonObject->TryGetArrayField(TEXT("keys"), KeyValues))
			{
				for (const TSharedPtr<FJsonValue>& KeyValue : *KeyValues)
				{
					if (!KeyValue.IsValid()) continue;
					const TSharedPtr<FJsonObject>* KeyObject;
					if (KeyValue->TryGetObject(KeyObject))
					{
						FString KeyStr = TEXT("");
						if (!KeyObject || !KeyObject->IsValid()) continue;
						KeyObject->Get()->TryGetStringField(TEXT("key"), KeyStr);
						Keys.AddUnique(KeyStr);
					}
				}
			}
		}

		CALLBACK(, Response, Keys);
	});
}

void UGamejoltSubsystem::OpenSession(TFunction<void(const FGamejoltResponse&)> Callback) const
{
	if (!IsLoggedIn()) CALLBACK(, GamejoltResponse::NotLoggedIn)
	MAKE_REQUEST(TEXT("sessions/open/?"), true, [Callback](const FGamejoltResponse& Response)
	{
		CALLBACK(, Response);
	});
}

void UGamejoltSubsystem::PingSession(const bool bIdle, TFunction<void(const FGamejoltResponse&)> Callback) const
{
	if (!IsLoggedIn()) CALLBACK(, GamejoltResponse::NotLoggedIn)
	MAKE_REQUEST(FString::Printf(TEXT("sessions/ping/?&status=%s"), bIdle ? TEXT("idle") : TEXT("active")), true,
		[Callback](const FGamejoltResponse& Response)
		{
			CALLBACK(, Response);
		});
}

void UGamejoltSubsystem::CheckSession(TFunction<void(const bool, const FGamejoltResponse&)> Callback) const
{
	if (!IsLoggedIn()) CALLBACK(, false, GamejoltResponse::NotLoggedIn)
	MAKE_REQUEST(TEXT("sessions/check/?"), true, [Callback](const FGamejoltResponse& Response)
	{
		CALLBACK(, Response.bSuccess, Response);
	});
}

void UGamejoltSubsystem::CloseSession(TFunction<void(const FGamejoltResponse&)> Callback) const
{
	if (!IsLoggedIn()) CALLBACK(, GamejoltResponse::NotLoggedIn)
	MAKE_REQUEST(TEXT("sessions/close/?"), true, [Callback](const FGamejoltResponse& Response)
	{
		CALLBACK(, Response);
	});
}

void UGamejoltSubsystem::AddTrophy(const int32 TrophyID, TFunction<void(const FGamejoltResponse&)> Callback) const
{
	MAKE_REQUEST(FString::Printf(TEXT("/trophies/add-achieved/?trophy_id=%d"), TrophyID), true,
		[Callback](const FGamejoltResponse& Response)
		{
			CALLBACK(, Response);
		});
}

void UGamejoltSubsystem::RemoveTrophy(const int32 TrophyID, TFunction<void(const FGamejoltResponse&)> Callback) const
{
	MAKE_REQUEST(FString::Printf(TEXT("/trophies/remove-achieved/?trophy_id=%d"), TrophyID), true,
		[Callback](const FGamejoltResponse& Response)
		{
			CALLBACK(, Response);
		});
}

void UGamejoltSubsystem::FetchTrophies(const EGamejoltTrophyFilter TypeFilter, const TArray<int32>& IDFilter, TFunction<void(const FGamejoltResponse&, const TArray<FGamejoltTrophyData>&)> Callback) const
{
	if (!IsLoggedIn()) CALLBACK(, GamejoltResponse::NotLoggedIn, {})

	FString BaseURL(TEXT("/trophies/?"));
	if (!IDFilter.IsEmpty())
	{
		BaseURL += TEXT("trophy_id=");
		for (int i = 0; i < IDFilter.Num(); i++)
		{
			BaseURL += FString::FromInt(IDFilter[i]);
			if (i < IDFilter.Num() - 1)	BaseURL += TEXT(",");
		}
	}
	else if (TypeFilter != EGamejoltTrophyFilter::AllTrophies)
	{
		BaseURL += FString::Printf(TEXT("achieved=%s"),
			TypeFilter == EGamejoltTrophyFilter::AchievedTrophies ? TEXT("true") : TEXT("false"));
	}
	
	MAKE_REQUEST(BaseURL, true, [Callback](const FGamejoltResponse& Response)
	{
		TArray<FGamejoltTrophyData> OutData;
		if (Response.bSuccess)
		{
			const TArray<TSharedPtr<FJsonValue>>* DataArray;
			Response.Json.JsonObject->TryGetArrayField(TEXT("trophies"), DataArray);
			if (DataArray && !DataArray->IsEmpty())
			{
				for (const TSharedPtr<FJsonValue>& DataValue : *DataArray)
				{
					if (!DataValue.IsValid()) continue;
					TSharedPtr<FJsonObject>* DataObject;
					DataValue->TryGetObject(DataObject);
					if (DataObject && DataObject->IsValid())
					{
						FGamejoltTrophyData Trophy;
						const TSharedPtr<FJsonObject> DerefObj = *DataObject;
						DerefObj->TryGetNumberField(TEXT("id"), Trophy.ID);
						DerefObj->TryGetStringField(TEXT("title"), Trophy.Name);
						DerefObj->TryGetStringField(TEXT("description"), Trophy.Description);
						DerefObj->TryGetStringField(TEXT("difficulty"), Trophy.Difficulty);
						DerefObj->TryGetStringField(TEXT("image_url"), Trophy.ImageURL);
						
						FString AchievedStr;
						DerefObj->TryGetStringField(TEXT("achieved"), AchievedStr);
						if (AchievedStr == TEXT("false"))
						{
							Trophy.bAchieved = false;
						}
						else
						{
							Trophy.bAchieved = true;
							Trophy.Timestamp = AchievedStr;
						}
						
						OutData.Emplace(Trophy);
					}
				}
			}
		}
		
		CALLBACK(, Response, OutData);
	});
}

void UGamejoltSubsystem::DeleteCredentials()
{
	Credentials.Key = TEXT("");
	Credentials.Value = TEXT("");
	if (FPaths::FileExists(GetCredentialsPath()))
	{
		IFileManager::Get().Delete(*GetCredentialsPath(), true, false, true);
	}
}

void UGamejoltSubsystem::SaveCredentials() const
{
	TArray<uint8> CompressedData;
	if (!Credentials.Key.IsEmpty() && !Credentials.Value.IsEmpty())
	{
		FString Data = FString::Printf(TEXT("%s_&&_%s"), *Credentials.Key, *Credentials.Value);
		Data = AESHelpers::EncryptAES(Data, GamejoltAES);

		FBufferArchive ToBinary(true);
		ToBinary << Data;

		FOodleCompressedArray::CompressTArray(CompressedData, ToBinary,
			FOodleDataCompression::ECompressor::Kraken,
			FOodleDataCompression::ECompressionLevel::SuperFast);

		ToBinary.FlushCache();
		ToBinary.Close();

		Data = TEXT("");
	}
	else
	{
		CompressedData = {0};
	}

	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [CompressedData](){
		FFileHelper::SaveArrayToFile(CompressedData, *GetCredentialsPath());
	});

	CompressedData.Empty();
}

void UGamejoltSubsystem::LoadCredentials(const TFunction<void(const FGamejoltResponse&)>& Callback)
{
	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, Callback]()
	{
		TArray<uint8> CompressedData;
		if (FPaths::FileExists(GetCredentialsPath()))
		{
			FFileHelper::LoadFileToArray(CompressedData, *GetCredentialsPath());
		}
		AsyncTask(ENamedThreads::GameThread, [this, Callback, CompressedData]()
		{
			if (CompressedData.IsEmpty())
			{
				CALLBACK(, {false, TEXT("No existing data to load")})
			}
				
			TArray<uint8> UncompressedData;
			FOodleCompressedArray::DecompressToTArray(UncompressedData, CompressedData);
					
			FMemoryReader FromBinary(UncompressedData, true);
			FromBinary.Seek(0);

			FString Data;
			FromBinary << Data;
			Data = AESHelpers::DecryptAES(Data, GamejoltAES);

			FromBinary.FlushCache();
			FromBinary.Close();
				
			FString Name = TEXT(""), Key = TEXT("");
			Data.Split(TEXT("_&&_"), &Name, &Key, ESearchCase::CaseSensitive, ESearchDir::FromStart);
			AuthLogin(Name, Key, false, [Callback](const FGamejoltResponse& Response)
			{
				CALLBACK(, Response)
			});
				
			UncompressedData.Empty();
			Data = TEXT("");
		});
	});
}

UWorld* UGamejoltSubsystem::GetWorld() const
{
	if (UWorld* World = Super::GetWorld())
	{
		return World;
	}

	return GEngine ? GEngine->GetCurrentPlayWorld() : GWorld;
}

FString UGamejoltSubsystem::GetGamejoltVersion() const
{
	if (!GetWorld()) return TEXT("v1_2");
	if (const UGTConfigSubsystem* Subsystem = UGTConfigSubsystem::Get(this))
	{
		return Subsystem->GetGamejoltVersion();
	}

	return TEXT("v1_2");
}

FString UGamejoltSubsystem::ConstructURL(const FString& URL, const bool bUser) const
{
	FString BaseURL(TEXT("https://api.gamejolt.com/api/game") / GetGamejoltVersion() / URL);
	FPaths::NormalizeDirectoryName(BaseURL);
	
	const FString UserData(FString::Printf(TEXT("&username=%s&user_token=%s"), *Credentials.Key, *Credentials.Value));
	const FString Combined = BaseURL + FString::Printf(TEXT("&game_id=%d%s"), GameData.Key, bUser && IsLoggedIn() ? *UserData : TEXT(""));
	return Combined + TEXT("&signature=") + FMD5::HashAnsiString(*(Combined + GameData.Value));
}

void UGamejoltSubsystem::CreateRequest(const FString& URL, TFunction<void(const FGamejoltResponse&)> Callback) const
{
	if (const FHttpRequestPtr NewRequest = FHttpModule::Get().CreateRequest(); NewRequest.IsValid())
	{
		NewRequest->OnProcessRequestComplete().BindLambda([Callback](const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, const bool bSuccess)
		{
			if (!Request.IsValid() || !Response.IsValid() || !bSuccess)
			{
				CALLBACK(, GamejoltResponse::InvalidURL)
			}

			FJsonObjectWrapper Json;
			Json.JsonObjectFromString(Response->GetContentAsString().Replace(
				TEXT("{\"response\":"), TEXT("")).LeftChop(1));
			
			Json.JsonObject->SetStringField(TEXT("Request"), Request->GetURL());
			Json.JsonObject->SetStringField(TEXT("Response"), Response->GetContentAsString());
			
			UE_LOG(GamejoltAPI, Display, TEXT("Response: %s"), *Response->GetContentAsString());

			CALLBACK(, {Json})
		});

		NewRequest->SetURL(URL);
		NewRequest->SetVerb("GET");
		NewRequest->ProcessRequest();

		UE_LOG(GamejoltAPI, Display, TEXT("URL: %s"), *NewRequest->GetURL());
	}
	else
	{
		CALLBACK(, GamejoltResponse::Unknown);
	}
}

void UGamejoltSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	if (const UGamejoltSettings* Settings = UGamejoltSettings::Get())
	{
		GameData.Key = Settings->GameID;
		GameData.Value = Settings->GameKey;
	}
	
	if (UGamejoltSettings::GetConst()->bAutoCheckConnection)
	{
		TestConnection(true);
	}
}

FIntVector4 UGamejoltSubsystem::FormatGamejoltVersionData(const FString& InString)
{
	FIntVector4 Result;
	TArray<FString> VerNums;
	InString.ParseIntoArray(VerNums, TEXT("."));
	if (VerNums.IsValidIndex(0))
	{
		Result.X = FCString::Atoi(*VerNums[0]);
	}
	if (VerNums.IsValidIndex(1))
	{
		Result.Y = FCString::Atoi(*VerNums[1]);
	}
	if (VerNums.IsValidIndex(2))
	{
		Result.Z = FCString::Atoi(*VerNums[2]);
	}
	if (VerNums.IsValidIndex(3))
	{
		Result.W = FCString::Atoi(*VerNums[3]);
	}

	return Result;
}
