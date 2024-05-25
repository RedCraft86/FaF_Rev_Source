// Copyright (C) RedCraft86. All Rights Reserved.

#include "SaveSubsystem.h"

#define DIFFICULTY_CHANGED(Difficulty) \
	{ OnDifficultyChanged.Broadcast(Difficulty); OnDifficultyChangedBP.Broadcast(Difficulty); }

void USaveSubsystem::SaveGameData(const float InPlayTime) const
{
	if (GameDataObject)
	{
		GameDataObject->PlayTime += InPlayTime;
		GameDataObject->SaveToFile(nullptr);
		OnSaveStarted.Broadcast();
	}
}

void USaveSubsystem::LoadGameData() const
{
	if (GameDataObject) GameDataObject->LoadFromFile(nullptr);
}

void USaveSubsystem::DeleteGameData() const
{
	if (GameDataObject) GameDataObject->DeleteFile();
}

void USaveSubsystem::UnlockTransientKey(const FName InKey) const
{
	if (GameDataObject) GameDataObject->TransientKeys.Add(InKey);
}

bool USaveSubsystem::HasTransientKey(const FName InKey) const
{
	if (GameDataObject) return GameDataObject->TransientKeys.Contains(InKey);
	return false;
}

void USaveSubsystem::SetDifficulty(const EDifficultyMode InDifficulty) const
{
	if (GameDataObject)
	{
		if (InDifficulty != GameDataObject->Difficulty)
		{
			GameDataObject->Difficulty = InDifficulty;
			DIFFICULTY_CHANGED(InDifficulty);
		}
	}
}

EDifficultyMode USaveSubsystem::GetDifficulty() const
{
	if (GameDataObject) return GameDataObject->Difficulty;
	return EDifficultyMode::Normal;
}

void USaveSubsystem::SaveGlobalData() const
{
	if (GlobalDataObject)
	{
		GlobalDataObject->SaveToFile(nullptr);
		OnSaveStarted.Broadcast();
	}
}

void USaveSubsystem::LoadGlobalData() const
{
	if (GlobalDataObject) GlobalDataObject->LoadFromFile(nullptr);
}

void USaveSubsystem::DeleteGlobalData() const
{
	if (GlobalDataObject) GlobalDataObject->DeleteFile();
}

void USaveSubsystem::UnlockGlobalKey(const FName InKey, const bool bSave) const
{
	if (GlobalDataObject)
	{
		GlobalDataObject->GlobalKeys.Add(InKey);
		if (bSave)
		{
			GlobalDataObject->SaveToFile(nullptr);
			OnSaveStarted.Broadcast();
		}
	}
}

bool USaveSubsystem::HasGlobalKey(const FName InKey) const
{
	if (GlobalDataObject) return GlobalDataObject->GlobalKeys.Contains(InKey);
	return false;
}

TArray<FName> USaveSubsystem::GetUnlockedMenus() const
{
	if (GlobalDataObject) return GlobalDataObject->MenuKeys.Array();
	return {};
}

void USaveSubsystem::ReachEnding(const FName InEndingID, const bool bSave) const
{
	if (GlobalDataObject && GlobalDataObject->Endings.FindRef(InEndingID).GetTicks() <= 0)
	{
		GlobalDataObject->Endings.Add(InEndingID, FDateTime::Now());
		if (bSave)
		{
			GlobalDataObject->SaveToFile(nullptr);
			OnSaveStarted.Broadcast();
		}
	}
}

FDateTime USaveSubsystem::GetEndingReachedTime(const FName InEndingID) const
{
	if (GlobalDataObject) GlobalDataObject->Endings.FindOrAdd(InEndingID);
	return {0};
}

void USaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GameDataObject = NewObject<UGameSaveObject>(this, TEXT("GameDataObject"));
	GameDataObject->LoadFromFile(nullptr);
	
	GlobalDataObject = NewObject<UGlobalSaveObject>(this, TEXT("GlobalDataObject"));
	GlobalDataObject->LoadFromFile(nullptr);
}
