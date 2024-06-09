// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "AsyncActions/GamejoltAsyncAction.h"
#include "AsyncSessionOpen.generated.h"

UCLASS()
class GAMEJOLTAPI_API UAsyncSessionOpen final : public UGamejoltAsyncAction
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI|Sessions", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Open Session (Gamejolt)", Keywords = "open user game session gamejolt"))
		static UAsyncSessionOpen* OpenSession();

	UPROPERTY(BlueprintAssignable)
		FGamejoltAsyncActionDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FGamejoltAsyncActionDelegate OnFail;
	
private:

	virtual void Activate() override;
};
