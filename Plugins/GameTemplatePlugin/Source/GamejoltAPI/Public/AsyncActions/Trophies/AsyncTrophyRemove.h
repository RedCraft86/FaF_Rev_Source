// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "AsyncActions/GamejoltAsyncAction.h"
#include "AsyncTrophyRemove.generated.h"

UCLASS()
class GAMEJOLTAPI_API UAsyncTrophyRemove final : public UGamejoltAsyncAction
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI|Trophies", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Remove Trophy (Gamejolt)", Keywords = "take remove delete trophy gamejolt"))
		static UAsyncTrophyRemove* RemoveTrophy(const int32 TrophyID);

	UPROPERTY(BlueprintAssignable)
		FGamejoltAsyncActionDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FGamejoltAsyncActionDelegate OnFail;
	
private:

	virtual void Activate() override;
	
	int32 _TrophyID = 0;
};
