// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameSettingTypes.h"
#include "GameFramework/GameUserSettings.h"
#include "GameSettings.generated.h"

UCLASS(BlueprintType)
class FAF_REV_API UGameSettings final : public UGameUserSettings
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetShowFPS(const bool bInShowFPS);

	UFUNCTION(BlueprintPure, Category = "Settings")
		bool GetShowFPS() const { return bShowFPS; }

	DECLARE_MULTICAST_DELEGATE(FOnSettingsApply);
	FOnSettingsApply OnDynamicApply, OnManualApply;

private:
	
	UPROPERTY(Transient) TMap<EFRSoundType, USoundClass*> SoundTypeToClass;
	UPROPERTY(Transient) class UFRGameInstance* GameInstance;
	UPROPERTY(Transient) class USoundMix* SoundMixObject;

// Configs
	
	UPROPERTY(Config) bool bShowFPS;
	UPROPERTY(Config) bool bSmoothCamera;
	UPROPERTY(Config) FVector2D SensitivityXY;
	UPROPERTY(Config) uint8 FieldOfView;
	
	UPROPERTY(Config) uint8 Brightness;
	UPROPERTY(Config) float Gamma;
	
	UPROPERTY(Config) EFRColorBlind ColorBlindMode;
	UPROPERTY(Config) uint8 ColorBlindStrength;
	
	UPROPERTY(Config) bool bFancyBloom;
	UPROPERTY(Config) uint8 MotionBlurAmount;
	UPROPERTY(Config) EFRAntiAliasing AntiAliasingMethod;
	
	UPROPERTY(Config) TMap<EFRSoundType, uint8> SoundTypeToVolume;

public: // Statics

	static UGameSettings* Get() { return Cast<UGameSettings>(GEngine->GameUserSettings); }
};
 