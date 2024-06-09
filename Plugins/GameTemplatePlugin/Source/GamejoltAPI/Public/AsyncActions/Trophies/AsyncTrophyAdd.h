// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "AsyncActions/GamejoltAsyncAction.h"
#include "AsyncTrophyAdd.generated.h"

UCLASS()
class GAMEJOLTAPI_API UAsyncTrophyAdd final : public UGamejoltAsyncAction
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI|Trophies", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Reward Trophy (Gamejolt)", Keywords = "add reward give trophy gamejolt"))
		static UAsyncTrophyAdd* AddTrophy(const int32 TrophyID);

	UPROPERTY(BlueprintAssignable)
		FGamejoltAsyncActionDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FGamejoltAsyncActionDelegate OnFail;
	
private:

	virtual void Activate() override;
	
	int32 _TrophyID = 0;
};
