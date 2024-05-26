// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "SaveObjects.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveSubsystem.generated.h"

UCLASS(BlueprintType)
class FAF_REV_API USaveSubsystem final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable, DisplayName = "On Difficulty Changed")
		FOnDifficultyChangedBP OnDifficultyChangedBP;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SaveManager")
		void SaveGameData(const float InPlayTime = 0.0f) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SaveManager")
		void LoadGameData() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SaveManager")
		void DeleteGameData() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SaveManager")
		void UnlockTransientKey(const FGameplayTag InKey) const;

	UFUNCTION(BlueprintPure, Category = "SaveManager")
		bool HasTransientKey(const FGameplayTag InKey) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SaveManager")
		void SetDifficulty(const EDifficultyMode InDifficulty) const;

	UFUNCTION(BlueprintPure, Category = "SaveManager")
		EDifficultyMode GetDifficulty() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SaveManager")
		void SaveGlobalData() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SaveManager")
		void LoadGlobalData() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SaveManager")
		void ReachEnding(const FGameplayTag InEndingID, const bool bSave = true) const;

	UFUNCTION(BlueprintPure, Category = "SaveManager")
		FDateTime GetEndingReachedTime(const FGameplayTag InEndingID) const;

	UFUNCTION(BlueprintPure, Category = "SaveManager")
		bool HasAnyEnding() const;

	UFUNCTION(BlueprintPure, Category = "SaveManager")
		TSet<FGameplayTag> GetUnlockedMenus() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SaveManager")
		void UnlockContent(const FGameplayTag InContentID, const bool bSave = true) const;

	UFUNCTION(BlueprintPure, Category = "SaveManager")
		bool IsContentUnlocked(const FGameplayTag InContentID) const;

	UFUNCTION(BlueprintPure, Category = "SaveManager")
		bool IsSectionNodeUnlocked(const FGuid InNodeID) const;
	
	DECLARE_MULTICAST_DELEGATE(FPromptSaveIcon)
	FPromptSaveIcon OnSaveStarted;
	
	FOnDifficultyChanged OnDifficultyChanged;
	
	UGameSaveObject* GetGameDataObject() const { return GameDataObject; }
	UGlobalSaveObject* GetGlobalDataObject() const { return GlobalDataObject; }

protected:

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SaveManager", meta = (AllowPrivateAccess = true))
		TObjectPtr<UGameSaveObject> GameDataObject;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SaveManager", meta = (AllowPrivateAccess = true))
		TObjectPtr<UGlobalSaveObject> GlobalDataObject;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public: // Statics
	
	static USaveSubsystem* Get(const UObject* WorldContext)
	{
		const UWorld* World = WorldContext ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
		const UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
		return GI ? GI->GetSubsystem<USaveSubsystem>() : nullptr;
	}
};
