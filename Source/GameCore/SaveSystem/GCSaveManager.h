// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "GCSaveManager.generated.h"

class UGCSaveObjectGame;
class UGCSaveObjectPersistent;
enum class EGCSaveGameError : uint8;

DECLARE_DYNAMIC_DELEGATE_OneParam(FGCSaveLoadCompleteSingleSignature, EGCSaveGameError, Error);

UCLASS(BlueprintType, DisplayName = "Save Manager")
class GAMECORE_API UGCSaveManager final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UGCSaveManager();

	static UGCSaveManager* Get(const UObject* WorldContext);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void SaveGameData(const TFunction<void(const EGCSaveGameError)>& OnFinished) const;
	void SavePersistentData(const TFunction<void(const EGCSaveGameError)>& OnFinished) const;

	UFUNCTION(BlueprintPure, Category = "SaveSystem|Manager")
		bool IsCreatingNewGame() const;
	
	UFUNCTION(BlueprintCallable, Category = "SaveSystem|Manager")
		void BeginUsingGameData(const uint8 InDifficulty) const;
	
	UFUNCTION(BlueprintCallable, Category = "SaveSystem|Manager", DisplayName = "Save Game Data")
		void BPSaveGameData(const FGCSaveLoadCompleteSingleSignature& OnComplete) const;

	UFUNCTION(BlueprintCallable, Category = "SaveSystem|Manager", DisplayName = "Save Persistent Data")
		void BPSavePersistentData(FGCSaveLoadCompleteSingleSignature OnComplete) const;

	UFUNCTION(BlueprintPure, Category = "SaveSystem|Manager")
		UGCSaveObjectGame* GetGameSaveObject() const;
	
	UFUNCTION(BlueprintPure, Category = "SaveSystem|Manager")
		UGCSaveObjectPersistent* GetPersistentData() const;
	
private:

	UPROPERTY(Transient)
		UGCSaveObjectGame* GameSaveObject;

	UPROPERTY(Transient)
		UGCSaveObjectPersistent* PersistentSaveObject;
};
