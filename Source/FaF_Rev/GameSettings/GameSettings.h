// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTGameUserSettings.h"
#include "GameSettings.generated.h"

UENUM(BlueprintType, DisplayName = "Sound Type")
enum class EFRSoundType : uint8
{
	Master,
	Music,
	SoundFX
};
ENUM_RANGE_BY_FIRST_AND_LAST(EFRSoundType, EFRSoundType::Master, EFRSoundType::SoundFX);

UCLASS(BlueprintType)
class FAF_REV_API UGameSettings final : public UGTGameUserSettings
{
	GENERATED_BODY()

	friend class UFRGameInstance;
	friend class USettingsWidgetBase;

public:

	UGameSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void InitializeSettings();
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetOverallQuality(const int32 InValue);

	UFUNCTION(BlueprintPure, Category = "Settings")
		int32 GetOverallQuality() const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetShowFPS(const bool bInShowFPS);

	UFUNCTION(BlueprintPure, Category = "Settings")
		bool GetShowFPS() const { return bShowFPS; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetUseSmoothCamera(const bool bUseSmoothCamera);

	UFUNCTION(BlueprintPure, Category = "Settings")
		bool GetUseSmoothCamera() const { return bSmoothCamera; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetSensitivityX(const float SensitivityX);
	
	UFUNCTION(BlueprintPure, Category = "Settings")
		float GetSensitivityX() const { return bInvertSensitivityX ? -SensitivityXY.X : SensitivityXY.X; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetInvertSensitivityX(const bool bInvertX);

	UFUNCTION(BlueprintCallable, Category = "Settings")
		bool GetInvertSensitivityX() const { return bInvertSensitivityX; }
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetSensitivityY(const float SensitivityY);

	UFUNCTION(BlueprintPure, Category = "Settings")
		float GetSensitivityY() const { return bInvertSensitivityY ? -SensitivityXY.Y : SensitivityXY.Y; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetInvertSensitivityY(const bool bInvertY);

	UFUNCTION(BlueprintCallable, Category = "Settings")
		bool GetInvertSensitivityY() const { return bInvertSensitivityY; }
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetFieldOfView(const uint8 InFieldOfView);

	UFUNCTION(BlueprintPure, Category = "Settings")
		uint8 GetFieldOfView() const { return FieldOfView; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetBrightness(const uint8 InBrightness);

	UFUNCTION(BlueprintPure, Category = "Settings")
		uint8 GetBrightness() const { return Brightness; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetUseFancyBloom(const bool bUseFancyBloom);

	UFUNCTION(BlueprintPure, Category = "Settings")
		bool GetUseFancyBloom() const { return bFancyBloom; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetAudioVolume(const EFRSoundType Type, const uint8 InVolume);

	UFUNCTION(BlueprintPure, Category = "Settings")
		uint8 GetAudioVolume(const EFRSoundType Type) { return SoundTypeToVolume.FindOrAdd(Type, 100); }

	UFUNCTION(BlueprintPure, Category = "Settings")
		float GetAudioVolumeValue(const EFRSoundType Type) { return GetAudioVolume(Type) * 0.01f; }
	
	DECLARE_MULTICAST_DELEGATE(FOnSettingsApply);
	FOnSettingsApply OnDynamicApply, OnManualApply;
	
	void SetFrameRateInt(const int32 InValue) { SetFrameRateLimit(IntToFrameRate(InValue)); }
	int32 GetFrameRateInt() const { return FrameRateToInt(GetFrameRateLimit()); }
	
	virtual UWorld* GetWorld() const override;

private:
	
	bool bInitializing, bLaunchWork;
	UPROPERTY(Transient) TObjectPtr<UFRGameInstance> GameInstance;
	
	UPROPERTY(Transient) TObjectPtr<USoundMix> SoundMixObject;
	UPROPERTY(Transient) TMap<EFRSoundType, TObjectPtr<USoundClass>> SoundTypeToClass;

	FName BrightnessParamName;
	UPROPERTY(Transient) TObjectPtr<UMaterialParameterCollection> BrightnessMPC;
	
// Configs
	
	UPROPERTY(Config) bool bShowFPS;
	UPROPERTY(Config) bool bSmoothCamera;
	UPROPERTY(Config) FVector2D SensitivityXY;
	UPROPERTY(Config) bool bInvertSensitivityX;
	UPROPERTY(Config) bool bInvertSensitivityY;
	UPROPERTY(Config) uint8 FieldOfView;
	UPROPERTY(Config) uint8 Brightness;
	UPROPERTY(Config) bool bFancyBloom;
	UPROPERTY(Config) TMap<EFRSoundType, uint8> SoundTypeToVolume;
	UPROPERTY(Config) TArray<uint8> ScalabilityDefaults;

	void CacheScalabilityDefaults();
	void ApplyBrightness() const;
	void ApplyAudioSettings();
	
	virtual void SetToDefaults() override;
	virtual void ApplyNonResolutionSettings() override;
	
public: // Statics
	
	static int32 FrameRateToInt(const float InFramerate);
	static float IntToFrameRate(const int32 InValue);
	
	static UGameSettings* Get() { return Cast<UGameSettings>(GEngine->GameUserSettings); }
};
 