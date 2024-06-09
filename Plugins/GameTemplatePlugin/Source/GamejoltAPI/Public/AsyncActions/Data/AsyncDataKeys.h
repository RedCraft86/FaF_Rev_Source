// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "AsyncActions/GamejoltAsyncAction.h"
#include "AsyncDataKeys.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGamejoltDataKeysDelegate, const TArray<FString>&, Keys, const FString&, Error);

UCLASS()
class GAMEJOLTAPI_API UAsyncDataKeys final : public UGamejoltAsyncAction
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI|Data", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Get Data Keys (Gamejolt)", Keywords = "get user global data keys gamejolt"))
		static UAsyncDataKeys* GetDataKeys(const bool bUserData);

	UPROPERTY(BlueprintAssignable)
		FGamejoltDataKeysDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FGamejoltDataKeysDelegate OnFail;
	
private:

	virtual void Activate() override;

	bool _User = false;
};
