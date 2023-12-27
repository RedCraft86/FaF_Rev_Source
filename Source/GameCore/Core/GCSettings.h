// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "GameCore.h"
#include "GameKeys.h"
#include "Engine/DeveloperSettings.h"
#include "Inventory/GCInventoryTypes.h"
#include "Materials/MaterialParameterCollection.h"
#include "UserSettings/GCUserSettingTypes.h"
#include "Styles/ExpressiveTextStyleBase.h"
#include "GCSettings.generated.h"

class USoundBase;

UCLASS(Config = Game, DefaultConfig, BlueprintType, NotBlueprintable, DisplayName = "Game Settings")
class GAMECORE_API UGCSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UGCSettings()
	{
		CategoryName = TEXT("Game");
		SectionName = TEXT("GCSettings");

		MusicFadeTime = 1.0f;
		DefaultGameMusic = nullptr;
		DefaultChaseMusic = nullptr;

		GameSequenceTable = nullptr;
		GameSequenceOrder = nullptr;

		BaseTextStyle = nullptr;

		DifficultyTable_AI = nullptr;

		InventoryItemTable = nullptr;
		InventoryLightingChannel.bChannel0 = false;
		InventoryLightingChannel.bChannel1 = true;
		InventoryLightingChannel.bChannel2 = false;
		InventoryDefaultItemID = {NAME_None};

		AchievementTable = nullptr;

		InfoPageTable = nullptr;

		InputMappingContext = nullptr;
		SoundMixClass = nullptr;
		for (const EGCSoundType Type : TEnumRange<EGCSoundType>())
		{
			SoundClasses.Add(Type);
		}

		MainRenderingMPC = nullptr;
		BrightnessParamName = NAME_None;

		GameID = 0;
#if WITH_EDITORONLY_DATA
		bUpdate = false;
#endif
	}

	static UGCSettings* Get()
	{
		UGCSettings* Settings = GetMutableDefault<UGCSettings>();
		checkf(Settings, TEXT("Could not get UGCSettings!"));
		return Settings;
	}

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "GameMusicData", meta = (ClampMin = 0.25f, UIMin = 0.25f))
		float MusicFadeTime;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "GameMusicData")
		TSoftObjectPtr<USoundBase> DefaultGameMusic;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "GameMusicData")
		TSoftObjectPtr<USoundBase> DefaultChaseMusic;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "GameSequenceData", meta = (RequiredAssetDataTags = "RowStructure=/Script/GameCore.GCSequenceData"))
		TSoftObjectPtr<UDataTable> GameSequenceTable;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "GameSequenceData")
		TSoftObjectPtr<class UGCSequenceOrderAsset> GameSequenceOrder;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "ExpressiveText")
		TSoftObjectPtr<UExpressiveTextStyleBase> BaseTextStyle;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Difficulty", meta = (RequiredAssetDataTags = "RowStructure=/Script/GameCore.GCAIDifficultyData"))
		TSoftObjectPtr<UDataTable> DifficultyTable_AI;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "InventoryData", meta = (RequiredAssetDataTags = "RowStructure=/Script/GameCore.GCInvItemData"))
		TSoftObjectPtr<UDataTable> InventoryItemTable;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "InventoryData")
		FLightingChannels InventoryLightingChannel;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "InventoryData")
		FGCInvItemID InventoryDefaultItemID;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "AchievementData", meta = (RequiredAssetDataTags = "RowStructure=/Script/GameCore.GCAchievementData"))
		TSoftObjectPtr<UDataTable> AchievementTable;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "InfoBookData", meta = (RequiredAssetDataTags = "RowStructure=/Script/GameCore.GCInfoPageData"))
		TSoftObjectPtr<UDataTable> InfoPageTable;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "UserSettings")
		TSoftObjectPtr<class UInputMappingContext> InputMappingContext;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "UserSettings")
		TSoftObjectPtr<class USoundMix> SoundMixClass;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "UserSettings", meta = (ReadOnlyKeys))
		TMap<EGCSoundType, TSoftObjectPtr<class USoundClass>> SoundClasses;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "UserSettings|Post Processing")
		TSoftObjectPtr<class UMaterialParameterCollection> MainRenderingMPC;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "UserSettings|Post Processing", meta = (GetOptions = "MPCScalarParamNames"))
		FName BrightnessParamName;
	
	UPROPERTY(Config, EditAnywhere, Category = "GamejoltAPI")
		int32 GameID;
	
	UPROPERTY(Config, VisibleAnywhere, Category = "GamejoltAPI")
		FString GameKey;
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "GamejoltAPI")
		FString PrivateKey;

	UPROPERTY(EditAnywhere, Category = "GamejoltAPI")
		bool bUpdate;
#endif
	
#if WITH_EDITOR
	DECLARE_MULTICAST_DELEGATE_TwoParams(FPreSettingsChange, UObject*, FProperty*);
	FPreSettingsChange PreSettingsChange;
	
private:

#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient, meta = (TransientToolProperty))
		TArray<FName> MPCScalarParamNames = {};

	UPROPERTY(Transient, meta = (TransientToolProperty))
		TArray<FName> MPCVectorParamNames = {};
#endif
	
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override
	{
		Super::PreEditChange(PropertyAboutToChange);
		if (!PropertyAboutToChange)
			return;
		
		if (PreSettingsChange.IsBound())
		{
			PreSettingsChange.Broadcast(this, PropertyAboutToChange);
		}
	}

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);
#if WITH_EDITORONLY_DATA
		if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UGCSettings, MainRenderingMPC))
		{
			MPCScalarParamNames.Empty();
			MPCVectorParamNames.Empty();
			if (const UMaterialParameterCollection* MPC = MainRenderingMPC.LoadSynchronous())
			{
				MPC->GetParameterNames(MPCScalarParamNames, false);
				MPC->GetParameterNames(MPCVectorParamNames, true);
			}
		}
#endif
#if WITH_EDITOR
		if (bUpdate)
		{
			bUpdate = false;
			if (PrivateKey.IsEmpty())
			{
				PrivateKey = FGameCoreModule::DecryptAES(GameKey, GameKeys::GamejoltAES);
			}
			else
			{
				GameKey = FGameCoreModule::EncryptAES(PrivateKey, GameKeys::GamejoltAES);
				PrivateKey = FGameCoreModule::DecryptAES(GameKey, GameKeys::GamejoltAES);
				
				TryUpdateDefaultConfigFile();
			}
		}
#endif
	}
#endif
};
