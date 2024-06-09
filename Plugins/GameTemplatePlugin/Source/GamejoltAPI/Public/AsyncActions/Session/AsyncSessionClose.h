// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "AsyncActions/GamejoltAsyncAction.h"
#include "AsyncSessionClose.generated.h"

UCLASS()
class GAMEJOLTAPI_API UAsyncSessionClose final : public UGamejoltAsyncAction
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI|Sessions", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Close Session (Gamejolt)", Keywords = "close user game session gamejolt"))
		static UAsyncSessionClose* CloseSession();

	UPROPERTY(BlueprintAssignable)
		FGamejoltAsyncActionDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FGamejoltAsyncActionDelegate OnFail;
	
private:

	virtual void Activate() override;
};
