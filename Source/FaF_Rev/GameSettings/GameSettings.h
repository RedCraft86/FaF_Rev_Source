// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameSettingTypes.h"
#include "GameFramework/GameUserSettings.h"
#include "GameSettings.generated.h"

UCLASS(BlueprintType)
class FAF_REV_API UGameSettings final : public UGameUserSettings
{
	GENERATED_BODY()

	friend class UFRGameInstance;

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
		float GetSensitivityX() const { return SensitivityXY.X; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetSensitivityY(const float SensitivityY);

	UFUNCTION(BlueprintPure, Category = "Settings")
		float GetSensitivityY() const { return SensitivityXY.Y; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetFieldOfView(const uint8 InFieldOfView);

	UFUNCTION(BlueprintPure, Category = "Settings")
		uint8 GetFieldOfView() const { return FieldOfView; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetBrightness(const uint8 InBrightness);

	UFUNCTION(BlueprintPure, Category = "Settings")
		uint8 GetBrightness() const { return Brightness; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetGamma(const float InGamma);

	UFUNCTION(BlueprintPure, Category = "Settings")
		float GetGamma() const { return Gamma; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetColorBlindMode(const EFRColorBlindMode InColorBlindMode);

	UFUNCTION(BlueprintPure, Category = "Settings")
		EFRColorBlindMode GetColorBlindMode() const { return ColorBlindMode; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetColorBlindStrength(const uint8 InColorBlindStrength);

	UFUNCTION(BlueprintPure, Category = "Settings")
		uint8 GetColorBlindStrength() const { return ColorBlindStrength; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetUseFancyBloom(const float bUseFancyBloom);

	UFUNCTION(BlueprintPure, Category = "Settings")
		bool GetUseFancyBloom() const { return bFancyBloom; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetMotionBlurAmount(const uint8 InMotionBlurAmount);

	UFUNCTION(BlueprintPure, Category = "Settings")
		uint8 GetMotionBlurAmount() const { return MotionBlurAmount; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetAntiAliasingMethod(const EFRAntiAliasingMethod InAntiAliasingMethod);

	UFUNCTION(BlueprintPure, Category = "Settings")
		EFRAntiAliasingMethod GetAntiAliasingMethod() const { return AntiAliasingMethod; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetAudioVolume(const EFRSoundType Type, const uint8 InVolume);

	UFUNCTION(BlueprintPure, Category = "Settings")
		uint8 GetAudioVolume(const EFRSoundType Type) { return SoundTypeToVolume.FindOrAdd(Type, 100); }

	UFUNCTION(BlueprintPure, Category = "Settings")
		float GetAudioVolumeValue(const EFRSoundType Type) { return GetAudioVolume(Type) * 0.01f; }
	
	DECLARE_MULTICAST_DELEGATE(FOnSettingsApply);
	FOnSettingsApply OnDynamicApply, OnManualApply;

	void SetResScalePercent(const float InValue) { SetResolutionScaleNormalized(InValue * 0.01); }
	float GetResScalePercent() const { return GetResolutionScaleNormalized() * 100.0f; }
	void SetFrameRateInt(const int32 InValue) { SetFrameRateLimit(IntToFrameRate(InValue)); }
	int32 GetFrameRateInt() const { return FrameRateToInt(GetFrameRateLimit()); }
	void SetAAliasingInt(const int32 InValue) { SetAntiAliasingMethod(static_cast<EFRAntiAliasingMethod>(InValue)); }
	int32 GetAAliasingInt() const { return static_cast<int32>(GetAntiAliasingMethod()); }
	void SetCBlindModeInt(const int32 InValue) { SetColorBlindMode(static_cast<EFRColorBlindMode>(InValue)); }
	int32 GetCBlindModeInt() const { return static_cast<int32>(GetColorBlindMode()); }

	TArray<FIntPoint> GetAllResolutions() const;
	virtual UWorld* GetWorld() const override;

private:

	bool bInitializing, bLaunchWork;
	UPROPERTY(Transient) UFRGameInstance* GameInstance;
	
	UPROPERTY(Transient) USoundMix* SoundMixObject;
	UPROPERTY(Transient) TMap<EFRSoundType, USoundClass*> SoundTypeToClass;

	FName BrightnessParamName;
	UPROPERTY(Transient) UMaterialParameterCollection* BrightnessMPC;
	
// Configs
	
	UPROPERTY(Config) bool bShowFPS;
	UPROPERTY(Config) bool bSmoothCamera;
	UPROPERTY(Config) FVector2D SensitivityXY;
	UPROPERTY(Config) uint8 FieldOfView;
	
	UPROPERTY(Config) uint8 Brightness;
	UPROPERTY(Config) float Gamma;
	
	UPROPERTY(Config) EFRColorBlindMode ColorBlindMode;
	UPROPERTY(Config) uint8 ColorBlindStrength;
	
	UPROPERTY(Config) bool bFancyBloom;
	UPROPERTY(Config) uint8 MotionBlurAmount;
	UPROPERTY(Config) EFRAntiAliasingMethod AntiAliasingMethod;
	
	UPROPERTY(Config) TMap<EFRSoundType, uint8> SoundTypeToVolume;

	void ApplyAudioSettings();
	void ApplyColorBlindSettings();

	virtual void SetToDefaults() override;
	virtual void ApplyNonResolutionSettings() override;
	
public: // Statics

	static int32 ConvertAAMethod(const EFRAntiAliasingMethod InMethod);
	static int32 FrameRateToInt(const float InFramerate);
	static float IntToFrameRate(const int32 InValue);
	
	static UGameSettings* Get() { return Cast<UGameSettings>(GEngine->GameUserSettings); }
};
 