// Copyright (C) RedCraft86. All Rights Reserved.

#include "SaveSubsystem.h"

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

void USaveSubsystem::UnlockContent(const TSet<FGameplayTag>& InContentIDs, const bool bSave) const
{
	if (GlobalDataObject)
	{
		GlobalDataObject->Content.Append(InContentIDs);
		if (bSave)
		{
			GlobalDataObject->SaveToFile(nullptr);
			OnSaveStarted.Broadcast();
		}
	}
}

void USaveSubsystem::ReachEnding(const FGameplayTag InEndingID, const bool bSave) const
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

void USaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GameDataObject = NewObject<UGameSaveObject>(this, TEXT("GameDataObject"));
	GameDataObject->LoadFromFile(nullptr);
	
	GlobalDataObject = NewObject<UGlobalSaveObject>(this, TEXT("GlobalDataObject"));
	GlobalDataObject->LoadFromFile(nullptr);
}
