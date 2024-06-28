// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GamejoltData.h"
#include "Subsystems/EngineSubsystem.h"
#include "GamejoltSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGamejoltCheckDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGamejoltAsyncActionDelegate, const FString&, Error);

UCLASS(BlueprintType)
class GAMEJOLTAPI_API UGamejoltSubsystem final : public UEngineSubsystem
{
	GENERATED_BODY()

public:

	UGamejoltSubsystem();

	UPROPERTY(BlueprintAssignable)
		FGamejoltCheckDelegate OnConnectionSuccessful;
	
	UPROPERTY(BlueprintAssignable)
		FGamejoltCheckDelegate OnConnectionTimedOut;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "GamejoltAPI")
		void DeleteCredentials() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "GamejoltAPI")
		void SaveCredentials() const;
	
	UFUNCTION(BlueprintPure, Category = "GamejoltAPI")
		void LoadCredentials(FString& OutName, FString& OutKey) const;
	
	void GetUserData(TFunction<void(const FGamejoltResponse&, const FGamejoltUserData&)> Callback) const;
	void AuthLogin(const FString& UserName, const FString& UserToken, const bool bRemember, TFunction<void(const FGamejoltResponse&)> Callback);
	void Logout(const bool bForget);
	bool IsLoggedIn() const;
	
	void DeleteData(const FString& Key, const bool bUser, TFunction<void(const FGamejoltResponse&)> Callback) const;
	void SetData(const FString& Key, const FString& Value, const bool bUser, TFunction<void(const FGamejoltResponse&)> Callback) const;
	void GetData(const FString& Key, const bool bUser, TFunction<void(const FGamejoltResponse&, const FString&)> Callback) const;
	void GetDataKeys(const bool bUser, TFunction<void(const FGamejoltResponse&, const TArray<FString>&)> Callback) const;

	void OpenSession(TFunction<void(const FGamejoltResponse&)> Callback) const;
	void PingSession(const bool bIdle, TFunction<void(const FGamejoltResponse&)> Callback) const;
	void CheckSession(TFunction<void(const bool, const FGamejoltResponse&)> Callback) const;
	void CloseSession(TFunction<void(const FGamejoltResponse&)> Callback) const;

	void AddTrophy(const int32 TrophyID, TFunction<void(const FGamejoltResponse&)> Callback) const;
	void RemoveTrophy(const int32 TrophyID, TFunction<void(const FGamejoltResponse&)> Callback) const;
	void FetchTrophies(const EGamejoltTrophyFilter TypeFilter, const TArray<int32>& IDFilter, TFunction<void(const FGamejoltResponse&, const TArray<FGamejoltTrophyData>&)> Callback) const;

	virtual UWorld* GetWorld() const override;

private:
	
	uint8 TestConnectionCount;
	TTuple<int32, FString> GameData;
	TTuple<FString, FString> Credentials;

	FString GetGamejoltVersion() const;
	FString ConstructURL(const FString& URL, const bool bUser) const;
	void CheckConnetionInternal(TFunction<void(const bool&)> Callback) const;
	void CreateRequest(const FString& URL, TFunction<void(const FGamejoltResponse&)> Callback) const;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public: // Statics

	UFUNCTION(BlueprintPure, Category = "GamejoltAPI")
		static FString GetGamejoltVersionDataKey() { return TEXT("version"); }

	UFUNCTION(BlueprintPure, Category = "GamejoltAPI")
		static FIntVector4 FormatGamejoltVersionData(const FString& InString);

	static FString GetCredentialsPath() { return FPaths::ProjectSavedDir() / TEXT("user-cache.gjc"); }
	static UGamejoltSubsystem* Get() { return GEngine ? GEngine->GetEngineSubsystem<UGamejoltSubsystem>() : nullptr; }
};
