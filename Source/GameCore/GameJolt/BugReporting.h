// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "GameVersion.h"
#include "JsonObjectWrapper.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "BugReporting.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGamejoltGetBans, const TArray<FString>&, UserIDs);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGamejoltBugReport, const FString&, FailReason, const bool, bBanned);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGamejoltGetReport, const FJsonObjectWrapper&, Data);

UCLASS()
class GAMECORE_API UGJBanUserID final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI|BugReporter", meta = (BlueprintInternalUseOnly = "true"))
		static UGJBanUserID* BanUserID(const FString UserToBan);

	UPROPERTY(BlueprintAssignable)
		FGamejoltFinish OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FGamejoltFinish OnFail;
	
	virtual void Activate() override;
	
private:
	
	bool bRunning = false;
	FString UserID = TEXT("");
};

UCLASS()
class GAMECORE_API UGJUnbanUserID final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI|BugReporter", meta = (BlueprintInternalUseOnly = "true"))
	static UGJUnbanUserID* UnbanUserID(const FString UserToUnban);

	UPROPERTY(BlueprintAssignable)
		FGamejoltFinish OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FGamejoltFinish OnFail;
	
	virtual void Activate() override;
	
private:
	
	bool bRunning = false;
	FString UserID = TEXT("");
};

UCLASS()
class GAMECORE_API UGJGetBannedUserIDs final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI|BugReporter", meta = (BlueprintInternalUseOnly = "true"))
	static UGJGetBannedUserIDs* GetBannedUserIDs();

	UPROPERTY(BlueprintAssignable)
		FGamejoltGetBans OnFinish;
	
	virtual void Activate() override;
	
private:
	
	bool bRunning = false;
};

UCLASS()
class GAMECORE_API UReportBug final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI|BugReporter", meta = (BlueprintInternalUseOnly = "true"))
		static UReportBug* ReportBug(const FJsonObjectWrapper& ReportData);

	UFUNCTION(BlueprintPure, Category = "GamejoltAPI|BugReporter", meta = (WorldContext = "WorldContextObject"))
		static FJsonObjectWrapper CreateBugReportData(const UObject* WorldContextObject, const FString& Title, const FString& Description, const bool bLevel, const bool bQuest, const bool bPosition);

	UPROPERTY(BlueprintAssignable)
		FGamejoltBugReport OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FGamejoltBugReport OnFail;
	
	virtual void Activate() override;
	
private:
	
	bool bRunning = false;
	FJsonObjectWrapper Data;
};

UCLASS()
class GAMECORE_API UGetBugReport final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI|BugReporter", meta = (BlueprintInternalUseOnly = "true"))
		static UGetBugReport* GetBugReport(const FString InKey);

	UPROPERTY(BlueprintAssignable)
		FGamejoltGetReport OnFinish;
	
	virtual void Activate() override;
	
private:
	
	bool bRunning = false;
	FString Key = TEXT("");
};
