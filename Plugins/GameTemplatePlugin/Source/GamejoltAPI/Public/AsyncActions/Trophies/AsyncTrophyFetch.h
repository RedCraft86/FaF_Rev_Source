// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "AsyncActions/GamejoltAsyncAction.h"
#include "AsyncTrophyFetch.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGamejoltDataTrophiesDelegate, const TArray<FGamejoltTrophyData>&, Trophies, const FString&, Error);

UCLASS()
class GAMEJOLTAPI_API UAsyncTrophyFetch final : public UExtendedAsyncAction
{
	GENERATED_BODY()

	
public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI|Trophies", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Fetch Trophies (Gamejolt)", Keywords = "get fetch trophy gamejolt"))
		static UAsyncTrophyFetch* FetchTrophies(const EGamejoltTrophyFilter TypeFilter, const TArray<int32>& ID_Filter);

	UPROPERTY(BlueprintAssignable)
		FGamejoltDataTrophiesDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FGamejoltDataTrophiesDelegate OnFail;
	
private:

	virtual void Activate() override;

	EGamejoltTrophyFilter _TypeFilter = EGamejoltTrophyFilter::AllTrophies;
	TArray<int32> _IDFilter = {};
};
