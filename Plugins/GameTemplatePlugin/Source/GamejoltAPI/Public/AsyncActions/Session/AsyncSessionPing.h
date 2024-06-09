// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "AsyncActions/GamejoltAsyncAction.h"
#include "AsyncSessionPing.generated.h"

UCLASS()
class GAMEJOLTAPI_API UAsyncSessionPing final : public UGamejoltAsyncAction
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI|Sessions", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Ping Session (Gamejolt)", Keywords = "ping user game session gamejolt"))
		static UAsyncSessionPing* PingSession(const bool bIsIdling);

	UPROPERTY(BlueprintAssignable)
		FGamejoltAsyncActionDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FGamejoltAsyncActionDelegate OnFail;
	
private:

	virtual void Activate() override;

	bool _IsIdling = false;
};
