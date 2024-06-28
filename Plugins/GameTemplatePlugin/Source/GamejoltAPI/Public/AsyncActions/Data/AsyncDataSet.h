// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "AsyncActions/GamejoltAsyncAction.h"
#include "AsyncDataSet.generated.h"

UCLASS()
class GAMEJOLTAPI_API UAsyncDataSet final : public UExtendedAsyncAction
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI|Data", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Set Data (Gamejolt)", Keywords = "set user global data gamejolt"))
		static UAsyncDataSet* SetData(const FString& Key, const FString& Value, const bool bUserData);

	UPROPERTY(BlueprintAssignable)
		FGamejoltAsyncActionDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FGamejoltAsyncActionDelegate OnFail;
	
private:

	virtual void Activate() override;
	
	FString _Key = TEXT("");
	FString _Value = TEXT("");
	bool _User = false;
};
