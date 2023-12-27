// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "GCAchievementTypes.h"
#include "Components/ActorComponent.h"
#include "GCAchievementManager.generated.h"

UCLASS(DisplayName = "Achievement Manager", NotBlueprintable, ClassGroup = (Gameplay), meta = (BlueprintSpawnableComponent))
class GAMECORE_API UGCAchievementManager final : public UActorComponent
{
	GENERATED_BODY()

public:

	UGCAchievementManager();

	static UGCAchievementManager* Get(const UObject* WorldContext);
	
	UFUNCTION(BlueprintCallable, Category = "Achievements|Manager")
		void AddAchievementProgress(const FGCAchievementID InAchievementID, const int32 InProgress = 1) const;

	UFUNCTION(BlueprintPure, Category = "Achievements|Manager")
		FIntPoint GetAchievementProgress(const FGCAchievementID InAchievementID) const;

	UFUNCTION(BlueprintPure, Category = "Achievements|Manager")
		bool IsAchievementComplete(const FGCAchievementID InAchievementID) const;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnAchievementComplete, const FGCAchievementID&)
	FOnAchievementComplete OnAchievementComplete;
	
public: // Static functions

	UFUNCTION(BlueprintPure, Category = "Achievements")
		static bool IsValidAchievementID(const FGCAchievementID InAchievementID);
	
	UFUNCTION(BlueprintPure, Category = "Achievements")
		static FGCAchievementData GetAchievementData(const FGCAchievementID InAchievementID);

	UFUNCTION(BlueprintPure, Category = "Achievements")
		static bool IsValidAchievementData(const FGCAchievementData& InItemData);

	UFUNCTION(BlueprintPure, Category = "Achievements")
		static TArray<FGCAchievementID> GetSortedAchievementIDs();
};
