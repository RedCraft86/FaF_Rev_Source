// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "AsyncActions/GamejoltAsyncAction.h"
#include "AsyncDataGet.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGamejoltKeyedDataDelegate, const FString&, Value, const FString&, Error);

UCLASS()
class GAMEJOLTAPI_API UAsyncDataGet final : public UExtendedAsyncAction
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI|Data", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Get Data (Gamejolt)", Keywords = "get user global data gamejolt"))
		static UAsyncDataGet* GetData(const FString& Key, const bool bUserData);

	UPROPERTY(BlueprintAssignable)
		FGamejoltKeyedDataDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FGamejoltKeyedDataDelegate OnFail;
	
private:

	virtual void Activate() override;
	
	FString _Key = TEXT("");
	bool _User = false;
};
