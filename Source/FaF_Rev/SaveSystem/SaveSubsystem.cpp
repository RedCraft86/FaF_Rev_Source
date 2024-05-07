// Copyright (C) RedCraft86. All Rights Reserved.

#include "SaveSubsystem.h"

void USaveSubsystem::SaveGameData(const float InPlayTime) const
{
	if (GameDataObject)
	{
		GameDataObject->PlayTime += InPlayTime;
		GameDataObject->SaveToFile(nullptr);
	}
}

void USaveSubsystem::LoadGameData() const
{
	if (GameDataObject)
	{
		GameDataObject->LoadFromFile(nullptr);
	}
}

void USaveSubsystem::DeleteGameData() const
{
	if (GameDataObject)
	{
		GameDataObject->DeleteFile();
	}
}

void USaveSubsystem::SaveGlobalData() const
{
	if (GlobalDataObject)
	{
		GlobalDataObject->SaveToFile(nullptr);
	}
}

void USaveSubsystem::LoadGlobalData() const
{
	if (GlobalDataObject)
	{
		GlobalDataObject->LoadFromFile(nullptr);
	}
}

void USaveSubsystem::DeleteGlobalData() const
{
	if (GlobalDataObject)
	{
		GlobalDataObject->DeleteFile();
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
