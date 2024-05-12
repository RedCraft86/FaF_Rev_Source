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

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SaveManager")
		void SaveGameData(const float InPlayTime = 0.0f) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SaveManager")
		void LoadGameData() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SaveManager")
		void DeleteGameData() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SaveManager")
		void UnlockTransientKey(const FName InKey) const;

	UFUNCTION(BlueprintPure, Category = "SaveManager")
		bool HasTransientKey(const FName InKey) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SaveManager")
		void SaveGlobalData() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SaveManager")
		void LoadGlobalData() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SaveManager")
		void DeleteGlobalData() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SaveManager")
		void UnlockGlobalKey(const FName InKey, const bool bSave = true) const;

	UFUNCTION(BlueprintPure, Category = "SaveManager")
		bool HasGlobalKey(const FName InKey) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SaveManager")
		void ReachEnding(const FName InEndingID, const bool bSave = true) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SaveManager")
		FDateTime GetEndingReachedTime(const FName InEndingID) const;

	DECLARE_MULTICAST_DELEGATE(FPromptSaveIcon)
	FPromptSaveIcon OnSaveStarted;
	
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
