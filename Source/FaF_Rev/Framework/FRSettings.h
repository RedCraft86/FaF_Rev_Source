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

		bIsDemo = false;
		WordsPerSecond = 2;
	}

	UPROPERTY(Config, EditAnywhere, Category = "Default")
		bool bIsDemo;
	
	UPROPERTY(Config, EditAnywhere, Category = "Text", meta = (ClampMin = 1, UIMin = 1))
		uint8 WordsPerSecond;

	UPROPERTY(Config, EditAnywhere, Category = "Text", meta = (RequiredAssetDataTags = "RowStructure=/Script/FaF_Rev.GuideBookPageData"))
		TSoftObjectPtr<UDataTable> GuideTable;
	
	UPROPERTY(Config, EditAnywhere, Category = "GameSection")
		TSoftObjectPtr<UWorld> GameplayMap;

	UPROPERTY(Config, EditAnywhere, Category = "GameSection")
		TSoftObjectPtr<class UGameSectionGraph> GameSectionGraph;

	UPROPERTY(Config, EditAnywhere, Category = "GameMusic", meta = (GetOptions = "GetMusicTableKeys"))
		FName DefaultMusicID;
	
	UPROPERTY(Config, EditAnywhere, Category = "GameMusic", meta = (RequiredAssetDataTags = "RowStructure=/Script/FaF_Rev.GameMusicData"))
		TSoftObjectPtr<UDataTable> MusicTable;

	float CalcReadingTime(const FString& InStr) const
	{
		TArray<FString> Words; InStr.ParseIntoArray(Words, TEXT(" "));
		return Words.Num() / (float)WordsPerSecond;
	}

	bool IsGameplayMap(const UObject* WorldContext) const
	{
		return UGameplayStatics::GetCurrentLevelName(WorldContext) == GameplayMap.GetAssetName();
	}

#if WITH_EDITORONLY_DATA
private:
	UFUNCTION() TArray<FName> GetMusicTableKeys()
	{
		TArray<FName> Name = {};
		if (MusicTable.LoadSynchronous())
		{
			Name = MusicTable.LoadSynchronous()->GetRowNames();
			if (DefaultMusicID.IsNone() && Name.Num() > 0)
				DefaultMusicID = Name[0];
		}
		
		return Name;
	}
#endif
public: // Statics
	
	static UFRSettings* Get() { return GetMutableDefault<UFRSettings>(); }
	static const UFRSettings* GetConst() { return GetDefault<UFRSettings>(); }
};
