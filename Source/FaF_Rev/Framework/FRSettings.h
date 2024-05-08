// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Kismet/GameplayStatics.h"
#include "Engine/DeveloperSettings.h"
#include "GameSettings/GameSettings.h"
#include "Materials/MaterialParameterCollection.h"
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
	
	UPROPERTY(Config, EditAnywhere, Category = "GameSettings")
		TSoftObjectPtr<USoundMix> SoundMixClass;

	UPROPERTY(Config, EditAnywhere, Category = "GameSettings", meta = (ReadOnlyKeys))
		TMap<EFRSoundType, TSoftObjectPtr<USoundClass>> SoundClasses;

	UPROPERTY(Config, EditAnywhere, Category = "GameSettings|PostProcessing")
		TSoftObjectPtr<UMaterialParameterCollection> MainRenderingMPC;

	UPROPERTY(Config, EditAnywhere, Category = "GameSettings|PostProcessing", meta = (GetOptions = "GetMainRenderingMPCNames"))
		FName BrightnessParamName;
	
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
		TArray<FName> Names = {};
		if (MusicTable.LoadSynchronous())
		{
			Names = MusicTable.LoadSynchronous()->GetRowNames();
			if (DefaultMusicID.IsNone() && Names.Num() > 0)
				DefaultMusicID = Names[0];
		}
		
		return Names;
	}

	UFUNCTION() TArray<FName> GetMainRenderingMPCNames() const
	{
		TArray<FName> Names = {};
		if (MainRenderingMPC.LoadSynchronous())
		{
			MainRenderingMPC.LoadSynchronous()->GetParameterNames(Names, false);
		}
		
		return Names;
	}
#endif
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);
		if (MusicTable.IsNull()) DefaultMusicID = NAME_None;
		if (SoundClasses.Num() < 5)
		{
			for (const EFRSoundType Type : TEnumRange<EFRSoundType>())
			{
				if (!SoundClasses.Contains(Type)) SoundClasses.Add(Type);
			}
		}
	}
#endif
public: // Statics
	
	static UFRSettings* Get() { return GetMutableDefault<UFRSettings>(); }
	static const UFRSettings* GetConst() { return GetDefault<UFRSettings>(); }
};
