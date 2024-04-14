// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameMusic/GameMusicData.h"
#include "GameSection/Graph/GameSectionGraph.h"
#include "Styles/ExpressiveTextStyleBase.h"
#include "Engine/DeveloperSettings.h"
#include "FRSettings.generated.h"

UCLASS(Config = Game, DefaultConfig, DisplayName = "Game Project")
class FAF_REV_API UFRSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	
	UFRSettings()
	{
		CategoryName = TEXT("Project");
		SectionName = TEXT("Game Project");
	}

	UPROPERTY(Config, EditAnywhere, Category = "GameSection")
		TSoftObjectPtr<UWorld> GameplayMap;
	
	UPROPERTY(Config, EditAnywhere, Category = "GameSection")
		TSoftObjectPtr<UGameSectionGraph> GameSectionGraph;

	UPROPERTY(Config, EditAnywhere, Category = "Settings")
		TSoftObjectPtr<UGameMusicData> DefaultGameMusicData;
	
	UPROPERTY(Config, EditAnywhere, Category = "Settings")
		TSoftObjectPtr<UExpressiveTextStyleBase> DefaultExpressiveTextStyle;

public: // Statics
	
	static UFRSettings* Get() { return GetMutableDefault<UFRSettings>(); }
	static const UFRSettings* GetConst() { return GetDefault<UFRSettings>(); }
};
