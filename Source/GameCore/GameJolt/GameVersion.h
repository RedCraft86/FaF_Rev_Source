// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "RCRuntimeTypes.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GameVersion.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGamejoltFinish, const FString&, FailReason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGamejoltGameVersion, const FRCGameVersion&, GameVersion);

UCLASS()
class GAMECORE_API UGJSetLatestVersion final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI", meta = (BlueprintInternalUseOnly = "true"))
		static UGJSetLatestVersion* SetLatestVersion(const FRCGameVersion NewVersion);

	UPROPERTY(BlueprintAssignable)
		FGamejoltFinish OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FGamejoltFinish OnFail;
	
	virtual void Activate() override;

private:
	
	bool bRunning = false;
	FRCGameVersion Version;
};

UCLASS()
class GAMECORE_API UGJGetLatestVersion final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI", meta = (BlueprintInternalUseOnly = "true"))
		static UGJGetLatestVersion* GetLatestVersion();

	UPROPERTY(BlueprintAssignable)
		FGamejoltGameVersion OnFinished;
	
	virtual void Activate() override;
	
private:

	bool bRunning = false;
};
