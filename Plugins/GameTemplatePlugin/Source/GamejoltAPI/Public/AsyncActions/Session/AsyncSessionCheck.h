// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "AsyncActions/GamejoltAsyncAction.h"
#include "AsyncSessionCheck.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGamejoltCheckSessionDelegate, const bool, bActive, const FString&, Error);

UCLASS()
class GAMEJOLTAPI_API UAsyncSessionCheck final : public UGamejoltAsyncAction
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI|Sessions", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Check Session (Gamejolt)", Keywords = "check user game session gamejolt"))
		static UAsyncSessionCheck* CheckSession();

	UPROPERTY(BlueprintAssignable)
		FGamejoltCheckSessionDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FGamejoltCheckSessionDelegate OnFail;
	
private:

	virtual void Activate() override;
};
