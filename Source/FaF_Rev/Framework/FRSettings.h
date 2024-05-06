// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameMusicData.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DeveloperSettings.h"
#include "FRSettings.generated.h"

#define FRSettings UFRSettings::Get()

UCLASS(Config = Game, DefaultConfig, DisplayName = "Game Project")
class FAF_REV_API UFRSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	
	UFRSettings()
	{
		CategoryName = TEXT("Project");
		SectionName = TEXT("Game Project");

		WordsPerSecond = 2;
		MusicFadeTime = 2.0f;
	}

	UPROPERTY(Config, EditAnywhere, Category = "Text", meta = (ClampMin = 1, UIMin = 1))
		uint8 WordsPerSecond;

	UPROPERTY(Config, EditAnywhere, Category = "Text", meta = (RequiredAssetDataTags = "RowStructure=/Script/FaF_Rev.GuideBookPageData"))
		TSoftObjectPtr<UDataTable> GuideTable;
	
	UPROPERTY(Config, EditAnywhere, Category = "GameSection")
		TSoftObjectPtr<UWorld> GameplayMap;

	UPROPERTY(Config, EditAnywhere, Category = "GameSection")
		TSoftObjectPtr<class UGameSectionGraph> GameSectionGraph;

	UPROPERTY(Config, EditAnywhere, Category = "GameMusic")
		float MusicFadeTime;
	
	UPROPERTY(Config, EditAnywhere, Category = "GameMusic", meta = (RequiredAssetDataTags = "RowStructure=/Script/FaF_Rev.GameMusicData"))
		TSoftObjectPtr<UDataTable> MusicTable;

	UPROPERTY(Config, EditAnywhere, Category = "GameMusic")
		FGameMusicID DefaultMusicID;
	
	float CalcReadingTime(const FString& InStr) const
	{
		TArray<FString> Words; InStr.ParseIntoArray(Words, TEXT(" "));
		return Words.Num() / (float)WordsPerSecond;
	}

	bool IsGameplayMap(const UObject* WorldContext) const
	{
		return UGameplayStatics::GetCurrentLevelName(WorldContext) == GameplayMap.GetAssetName();
	}
	
public: // Statics
	
	static UFRSettings* Get() { return GetMutableDefault<UFRSettings>(); }
	static const UFRSettings* GetConst() { return GetDefault<UFRSettings>(); }
};
