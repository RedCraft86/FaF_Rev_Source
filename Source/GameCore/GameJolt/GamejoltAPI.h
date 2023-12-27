// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "GameKeys.h"
#include "HttpModule.h"
#include "RCRuntimeTypes.h"
#include "Core/GCSettings.h"
#include "JsonObjectWrapper.h"
#include "Interfaces/IHttpResponse.h"
#include "GenericPlatform/GenericPlatformHttp.h"

#define VERSION_KEY TEXT("version")
#define BAN_LIST_KEY TEXT("banList")

namespace GamejoltAPI
{
	typedef const TFunction<void(FJsonObjectWrapper&)>& RequestCallback;
		
	GAMECORE_API FORCEINLINE FString ConstructURL(const FString& URL)
	{
		if (const UGCSettings* Settings = UGCSettings::Get())
		{
			FString BaseURL{TEXT("https://api.gamejolt.com/api/game/v1_2") / URL};
			BaseURL += TEXT("&game_id=") + FString::FromInt(Settings->GameID);
			return BaseURL + "&signature=" + FMD5::HashAnsiString(*(BaseURL +
				FGameCoreModule::DecryptAES(Settings->GameKey, GameKeys::GamejoltAES)));
		}

		return TEXT("");
	}

	GAMECORE_API FORCEINLINE FHttpRequestPtr CreateRequest(const FString& URL, TFunction<void(const FJsonObjectWrapper&)> Callback)
	{
		if (FHttpRequestPtr Request = FHttpModule::Get().CreateRequest(); Request.IsValid())
		{
			Request->OnProcessRequestComplete().BindLambda([Callback](const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, const bool bSuccess)
		   {
				FJsonObjectWrapper Object;
				if (Response.IsValid() && Request.IsValid())
				{
					Object.JsonObjectFromString(Response->GetContentAsString());
					Object.JsonObject->SetStringField(TEXT("url"), Request->GetURL());
					Object.JsonObject->SetStringField(TEXT("verb"), Request->GetVerb());
					Callback(Object);
				}
				else
				{
					Callback(Object);
				}
		   });
			Request->SetVerb("GET");
			Request->SetURL(ConstructURL(URL));
			Request->ProcessRequest();
			return Request;
		}
		
		Callback({});
		return nullptr;
	}

	GAMECORE_API FORCEINLINE void SetData(const FString& Key, const FString& Value, const TFunction<void(const bool, const FString&)>& Callback)
	{
		const FString URL(FString::Printf(TEXT("/data-store/set/?&key=%s&data=%s"),
			*FGenericPlatformHttp::UrlEncode(Key), *FGenericPlatformHttp::UrlEncode(Value)));

		CreateRequest(URL, [Callback](const FJsonObjectWrapper& Object)
		{
			if (Object.JsonObject->HasField(TEXT("response")))
			{
				const TSharedPtr<FJsonObject>& Response = Object.JsonObject->GetObjectField(TEXT("response"));
				FString ErrorMsg = TEXT(""); Response->TryGetStringField("message", ErrorMsg);
				Callback(Response->GetBoolField("success"), ErrorMsg);
			}
			else
			{
				Callback(false, TEXT("No connection or other internal error."));
			}
		});	
	}

	GAMECORE_API FORCEINLINE void GetData(const FString& Key, const TFunction<void(const bool, const FString&, const FString&)>& Callback)
	{
		const FString URL(FString::Printf(TEXT("/data-store/?&key=%s"),
			*FGenericPlatformHttp::UrlEncode(Key)));

		CreateRequest(URL, [Callback](const FJsonObjectWrapper& Object)
		{
			if (Object.JsonObject->HasField(TEXT("response")))
			{
				const TSharedPtr<FJsonObject>& Response = Object.JsonObject->GetObjectField(TEXT("response"));
				FString ErrorMsg = TEXT(""); Response->TryGetStringField("message", ErrorMsg);
				
				const bool bSuccessful = Response->GetBoolField("success");
				Callback(bSuccessful, ErrorMsg, bSuccessful ? Response->GetStringField(TEXT("data")) : TEXT(""));
			}
			else
			{
				Callback(false, TEXT("No connection or other internal error."), TEXT(""));
			}
		});	
	}

	GAMECORE_API FORCEINLINE void DeleteData(const FString& Key, const TFunction<void(const bool, const FString&)>& Callback)
	{
		const FString URL(FString::Printf(TEXT("/data-store/remove/?&key=%s"),
			*FGenericPlatformHttp::UrlEncode(Key)));

		CreateRequest(URL, [Callback](const FJsonObjectWrapper& Object)
		{
			if (Object.JsonObject->HasField(TEXT("response")))
			{
				const TSharedPtr<FJsonObject>& Response = Object.JsonObject->GetObjectField(TEXT("response"));
				FString ErrorMsg = TEXT(""); Response->TryGetStringField("message", ErrorMsg);
				Callback(Response->GetBoolField("success"), ErrorMsg);
			}
			else
			{
				Callback(false, TEXT("No connection or other internal error."));
			}
		});	
	}

	GAMECORE_API FORCEINLINE void GetDataKeys(const TFunction<void(const bool, const FString&, const TArray<FString>&)>& Callback)
	{
		CreateRequest(TEXT("/data-store/get-keys/?"), [Callback](const FJsonObjectWrapper& Object)
		{
			if (Object.JsonObject->HasField(TEXT("response")))
			{
				const TSharedPtr<FJsonObject>& Response = Object.JsonObject->GetObjectField(TEXT("response"));
				FString ErrorMsg = TEXT(""); Response->TryGetStringField("message", ErrorMsg);
			
				const TArray<TSharedPtr<FJsonValue>>& KeyValues = Response->GetArrayField(TEXT("keys"));
				TArray<FString> Keys;
				for (const TSharedPtr<FJsonValue>& KeyVal : KeyValues)
				{
					Keys.Add(KeyVal->AsObject()->GetStringField("key"));
				}
			
				Callback(Response->GetBoolField(TEXT("success")), ErrorMsg, Keys);
			}
			else
			{
				Callback(false, TEXT("No connection or other internal error."), {});
			}
		});	
	}

	GAMECORE_API FORCEINLINE void GetLatestVersion(const TFunction<void(const FRCGameVersion&)>& Callback)
	{
		GetData(VERSION_KEY, [Callback](const bool, const FString&, const FString& Data)
		{
			Callback(FRCGameVersion(Data));
		});
	}

	GAMECORE_API FORCEINLINE void SetLatestVersion(const FRCGameVersion& Version, const TFunction<void(const bool, const FString&)>& Callback)
	{
		SetData(VERSION_KEY, Version.ToString(), Callback);
	}

	GAMECORE_API FORCEINLINE void BanUser(const FString& UserID, const TFunction<void(const bool, const FString&)>& Callback)
	{
		GetData(BAN_LIST_KEY, [UserID, Callback](const bool, const FString&, const FString& Data)
		{
			SetData(BAN_LIST_KEY,
				FString::Printf(TEXT("%s,%s"), *Data, *UserID).Replace(TEXT(",,"), TEXT(",")), Callback);
		});
	}

	GAMECORE_API FORCEINLINE void UnbanUser(const FString& UserID, const TFunction<void(const bool, const FString&)>& Callback)
	{
		GetData(BAN_LIST_KEY, [UserID, Callback](const bool, const FString&, const FString& Data)
		{
			FString Str = Data.Replace(*UserID, TEXT(""));
			while (Str.Contains(TEXT(",,")))
			{
				Str.ReplaceInline(TEXT(",,"), TEXT(""));
			}
			
			if (Str.IsEmpty() || Str.Equals(TEXT(",")) || Str.Equals(TEXT(", ")) || Str.Equals(TEXT(" ,")))
			{
				DeleteData(BAN_LIST_KEY, Callback);
			}
			else
			{
				SetData(BAN_LIST_KEY, Str, Callback);
			}
		});
	}

	GAMECORE_API FORCEINLINE void GetBugReport(const FString& Key, const TFunction<void(const bool, const FJsonObjectWrapper&)>& Callback)
	{
		if (!Key.Contains("bugReport_")) Callback(false, {});
		GetData(Key, [Callback](const bool bSuccess, const FString&, const FString& Data)
		{
			FJsonObjectWrapper DataJson; DataJson.JsonObjectFromString(Data);
			Callback(bSuccess, DataJson);
		});
	}

	GAMECORE_API FORCEINLINE void CreateBugReport(const FJsonObjectWrapper& ReportData, const TFunction<void(const bool, const bool, const FString&)>& Callback)
	{
		const FString UserID(FPlatformMisc::GetLoginId());
		GetData(BAN_LIST_KEY, [UserID, ReportData, Callback](const bool bSuccess, const FString& Response, const FString& Data)
		{
			if (!bSuccess)
			{
				Callback(false, false, Response);
			}
			
			if (Data.Contains(UserID))
			{
				Callback(false, true, TEXT("You are banned from reporting bugs to this game."));
			}
			else
			{
				if (!ReportData.JsonObject->HasField("title") || !ReportData.JsonObject->HasField("description"))
				{
					Callback(false, false, TEXT("No title and/or description."));
					return;
				}
				
				const FJsonObjectWrapper ReportJson = ReportData;
				ReportJson.JsonObject->SetStringField(TEXT("user"), UserID);
				ReportJson.JsonObject->SetStringField(TEXT("time"), FDateTime::Now().ToString());
				
				FString ReportStr; ReportJson.JsonObjectToString(ReportStr);
				SetData(FString::Printf(TEXT("bugReport_%s__%s"), *FDateTime::Now().ToString(), *UserID), ReportStr,
					[Callback](const bool bSuccessful, const FString& ResponseStr)
				{
					Callback(bSuccessful, false, ResponseStr);
				});
			}
		});
	}
}
