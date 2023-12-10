// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Inventory/GCInventoryTypes.h"
#include "GCSaveObjects.generated.h"

enum class EGCSaveGameError : uint8;

UCLASS(Abstract, NotBlueprintable)
class GAMECORE_API UGCSaveObjectBase : public UObject
{
	GENERATED_BODY()

public:

	UGCSaveObjectBase();
	
	EGCSaveGameError GetLastError() const { return LastError; }
	virtual FString GetSaveFilePath() { return FPaths::ProjectSavedDir() / TEXT("SaveGames") / FileName + TEXT(".sav"); }
	void SaveGame(const TFunction<void(const EGCSaveGameError)>& Callback);
	void LoadGame(const TFunction<void(const EGCSaveGameError)>& Callback);
	virtual void DeleteGame();
	
protected:

	FString FileName;
	EGCSaveGameError LastError;

	virtual void OnPreSave() {}
	virtual void SerializeData(FArchive& Ar) {}
};

UCLASS(DisplayName = "Game Save Object")
class GAMECORE_API UGCSaveObjectGame final : public UGCSaveObjectBase
{
	GENERATED_BODY()

public:

	UGCSaveObjectGame();

	UPROPERTY(BlueprintReadOnly, Category = "GameSaveObject")
		uint8 Difficulty;

	UPROPERTY(BlueprintReadOnly, Category = "GameSaveObject")
		FName GameSequenceID;

	UPROPERTY(BlueprintReadOnly, Category = "GameSaveObject")
		TMap<FName, uint8> SubsequenceIndexes;

	UPROPERTY(BlueprintReadOnly, Category = "GameSaveObject")
		TMap<FGameplayTag, FGCInvSaveData> InventoryData;
	
	UPROPERTY(BlueprintReadOnly, Category = "GameSaveObject")
		float TotalPlayTime;
	
	UPROPERTY(BlueprintReadOnly, Category = "GameSaveObject")
		FDateTime StartDate;
	
	UPROPERTY(BlueprintReadOnly, Category = "GameSaveObject")
		FDateTime EndDate;

	virtual void DeleteGame() override
	{
		Difficulty = 1;
		GameSequenceID = NAME_None;
		SubsequenceIndexes = {};
		InventoryData = {};
		TotalPlayTime = 0.0f;
		StartDate = {0};
		EndDate = {0};
		Super::DeleteGame();
	}
	
	void AllowTimeSaving();
	static UGCSaveObjectGame* CreateOrLoad(UObject* Outer, const FString& InFileName = TEXT("GameData"));
	
private:

	bool bShouldSaveTime;
	FDateTime LastSavedTime;

	virtual void OnPreSave() override;
	virtual void SerializeData(FArchive& Ar) override
	{
		Ar << GameSequenceID;
		Ar << SubsequenceIndexes;
		Ar << InventoryData;
		Ar << TotalPlayTime;
		Ar << StartDate;
		Ar << EndDate;
	}
};

UCLASS(DisplayName = "Persistent Save Object")
class GAMECORE_API UGCSaveObjectPersistent final : public UGCSaveObjectBase
{
	GENERATED_BODY()

public:

	UGCSaveObjectPersistent();
	
	UPROPERTY(BlueprintReadOnly, Category = "PersistentSaveObject")
		TSet<FName> UnlockedContent;
	
	UPROPERTY(BlueprintReadOnly, Category = "PersistentSaveObject")
		TMap<FName, uint8> Achievements;

	virtual void DeleteGame() override
	{
		UnlockedContent = {};
		Achievements = {};
		Super::DeleteGame();
	}
	
	static UGCSaveObjectPersistent* CreateOrLoad(UObject* Outer, const FString& InFileName = TEXT("PersistentData"));
	
private:

	virtual void SerializeData(FArchive& Ar) override
	{
		Ar << UnlockedContent;
		Ar << Achievements;
	}
};