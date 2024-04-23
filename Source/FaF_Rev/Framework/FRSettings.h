// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "Styles/ExpressiveTextStyleBase.h"
#include "FRSettings.generated.h"

#define FRSettings UFRSettings::Get()

UCLASS(Config = Game, DefaultConfig, DisplayName = "Game Project")
class FAF_REV_API UFRSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	
	UFRSettings() : WordsPerSecond(2)
	{
		CategoryName = TEXT("Project");
		SectionName = TEXT("Game Project");
	}

	UPROPERTY(Config, EditAnywhere, Category = "Default")
		TSoftObjectPtr<class UGameMusicData> DefaultMusic;

	UPROPERTY(Config, EditAnywhere, Category = "Text", meta = (ClampMin = 1, UIMin = 1))
		uint8 WordsPerSecond;
	
	UPROPERTY(Config, EditAnywhere, Category = "Text")
		TSoftObjectPtr<UExpressiveTextStyleBase> DefaultTextStyle;

	UPROPERTY(Config, EditAnywhere, Category = "Text", meta = (RequiredAssetDataTags = "RowStructure=/Script/FaF_Rev.GuideBookPageData"))
		TSoftObjectPtr<UDataTable> GuideTable;
	
	UPROPERTY(Config, EditAnywhere, Category = "GameSection")
		TSoftObjectPtr<UWorld> GameplayMap;

	UPROPERTY(Config, EditAnywhere, Category = "GameSection")
		TSoftObjectPtr<class UGameSectionGraph> GameSectionGraph;

	float CalcReadingTime(const FString& InStr) const
	{
		TArray<FString> Words; InStr.ParseIntoArray(Words, TEXT(" "));
		return Words.Num() / (float)WordsPerSecond;
	}
	
public: // Statics
	
	static UFRSettings* Get() { return GetMutableDefault<UFRSettings>(); }
	static const UFRSettings* GetConst() { return GetDefault<UFRSettings>(); }
};
