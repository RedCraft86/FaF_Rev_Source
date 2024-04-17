// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameMusic/GameMusicData.h"
#include "UserWidgets/LoadingWidget.h"
#include "GameSection/Graph/GameSectionGraph.h"
#include "Styles/ExpressiveTextStyleBase.h"
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
		
		ReadingWordsPerSecond = 2.0f;
	}

	UPROPERTY(Config, EditAnywhere, Category = "GameSection")
		TSoftObjectPtr<UWorld> GameplayMap;
	
	UPROPERTY(Config, EditAnywhere, Category = "GameSection")
		TSoftObjectPtr<UGameSectionGraph> GameSectionGraph;
		
	UPROPERTY(Config, EditAnywhere, Category = "GameSection")
		TSoftClassPtr<ULoadingWidgetBase> LoadingWidgetClass;
	
	UPROPERTY(Config, EditAnywhere, Category = "Settings")
		TSoftObjectPtr<UGameMusicData> DefaultGameMusicData;
	
	UPROPERTY(Config, EditAnywhere, Category = "Settings")
		TSoftObjectPtr<UExpressiveTextStyleBase> DefaultExpressiveTextStyle;

	UPROPERTY(Config, EditAnywhere, Category = "Settings", meta = (ClampMin = 1, UIMin = 1))
		uint8 ReadingWordsPerSecond;

public: // Statics
	
	static UFRSettings* Get() { return GetMutableDefault<UFRSettings>(); }
	static const UFRSettings* GetConst() { return GetDefault<UFRSettings>(); }
};
