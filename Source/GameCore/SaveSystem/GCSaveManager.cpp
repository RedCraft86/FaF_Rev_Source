// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "GCSaveManager.h"
#include "GCSaveObjects.h"
#include "GCSaveTypes.h"
#include "Core/GCSettings.h"
#include "GameSequence/GCSequenceManager.h"

UGCSaveManager::UGCSaveManager()
{
	GameSaveObject = nullptr;
	PersistentSaveObject = nullptr;
}

UGCSaveManager* UGCSaveManager::Get(const UObject* WorldContext)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	const UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
	return GI ? GI->GetSubsystem<UGCSaveManager>() : nullptr;
}

void UGCSaveManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GameSaveObject = UGCSaveObjectGame::CreateOrLoad(this);
	PersistentSaveObject = UGCSaveObjectPersistent::CreateOrLoad(this);
}

void UGCSaveManager::SaveGameData(const TFunction<void(const EGCSaveGameError)>& OnFinished) const
{
	if (!GameSaveObject)
	{
		// I don't know how this can happen, given that the Save Objects are UPROPERTY(s) and the Save Manager is persistent.
		// But if the object ever does get garbage collected or simply don't load, we need to assert instead of report.
		// Attempting to reload the object is an option but if it keeps failing, we may end up with an infinite recursion.
		checkf(0, TEXT("Game Save Object is null."))
	}
	
	GameSaveObject->SaveGame(OnFinished);
}

void UGCSaveManager::SavePersistentData(const TFunction<void(const EGCSaveGameError)>& OnFinished) const
{
	if (!PersistentSaveObject)
	{
		// See comment on SaveGameData
		checkf(0, TEXT("Persistent Save Object is null."))
	}
	
	PersistentSaveObject->SaveGame(OnFinished);
}

bool UGCSaveManager::IsCreatingNewGame() const
{
	if (!GameSaveObject)
	{
		// See comment on SaveGameData
		checkf(0, TEXT("Game Save Object is null."))
	}

	return GameSaveObject->GameSequenceID.IsNone() && GameSaveObject->TotalPlayTime <= 0.0f;
}

void UGCSaveManager::BeginUsingGameData(const uint8 InDifficulty) const
{
	if (!GameSaveObject)
	{
		// See comment on SaveGameData
		checkf(0, TEXT("Game Save Object is null."))
	}
	
	if (IsCreatingNewGame())
	{
		GameSaveObject->TotalPlayTime = 0.1f;
		GameSaveObject->Difficulty = InDifficulty;
		GameSaveObject->SaveGame(nullptr);
	}
	
	GameSaveObject->AllowTimeSaving();
}

void UGCSaveManager::BPSaveGameData(const FGCSaveLoadCompleteSingleSignature& OnComplete) const
{
	SaveGameData([&OnComplete](const EGCSaveGameError Error){
		if (OnComplete.IsBound())
			OnComplete.Execute(Error);
	});
}

void UGCSaveManager::BPSavePersistentData(FGCSaveLoadCompleteSingleSignature OnComplete) const
{
	SavePersistentData([&OnComplete](const EGCSaveGameError Error){
		if (OnComplete.IsBound())
			OnComplete.Execute(Error);
	});
}

UGCSaveObjectGame* UGCSaveManager::GetGameSaveObject() const
{
	if (!GameSaveObject)
	{
		// See comment on SaveGameData
		checkf(0, TEXT("Game Save Object is null."))
	}
	
	return GameSaveObject;
}

UGCSaveObjectPersistent* UGCSaveManager::GetPersistentData() const
{
	if (!PersistentSaveObject)
	{
		// See comment on SaveGameData
		checkf(0, TEXT("Persistent Save Object is null."))
		return nullptr;
	}
	
	return PersistentSaveObject;
}
