// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "AsyncActions/GamejoltAsyncAction.h"
#include "AsyncDataDelete.generated.h"

UCLASS()
class GAMEJOLTAPI_API UAsyncDataDelete final : public UExtendedAsyncAction
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI|Data", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Delete Data (Gamejolt)", Keywords = "delete user global data gamejolt"))
		static UAsyncDataDelete* DeleteData(const FString& Key, const bool bUserData);

	UPROPERTY(BlueprintAssignable)
		FGamejoltAsyncActionDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FGamejoltAsyncActionDelegate OnFail;
	
private:

	virtual void Activate() override;
	
	FString _Key = TEXT("");
	bool _User = false;
};
