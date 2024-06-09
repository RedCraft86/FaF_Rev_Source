// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameSettingTypes.h"
#include "GameFramework/GameUserSettings.h"
#include "GTGameUserSettings.generated.h"

UCLASS()
class GTRUNTIME_API UGTGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetGamma(const float InGamma);

	UFUNCTION(BlueprintPure, Category = "Settings")
		float GetGamma() const { return Gamma; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetColorBlindMode(const EGTColorBlindMode InColorBlindMode);

	UFUNCTION(BlueprintPure, Category = "Settings")
		EGTColorBlindMode GetColorBlindMode() const { return ColorBlindMode; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetColorBlindStrength(const uint8 InColorBlindStrength);

	UFUNCTION(BlueprintPure, Category = "Settings")
		uint8 GetColorBlindStrength() const { return ColorBlindStrength; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetMotionBlurAmount(const uint8 InMotionBlurAmount);

	UFUNCTION(BlueprintPure, Category = "Settings")
		uint8 GetMotionBlurAmount() const { return MotionBlurAmount; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetAntiAliasingMethod(const EGTAntiAliasingMethod InAntiAliasingMethod);

	UFUNCTION(BlueprintPure, Category = "Settings")
		EGTAntiAliasingMethod GetAntiAliasingMethod() const { return AntiAliasingMethod; }

	void SetResScalePercent(const float InValue) { SetResolutionScaleNormalized(InValue * 0.01); }
	float GetResScalePercent() const { return GetResolutionScaleNormalized() * 100.0f; }
	void SetAAliasingInt(const int32 InValue) { SetAntiAliasingMethod(static_cast<EGTAntiAliasingMethod>(InValue)); }
	int32 GetAAliasingInt() const { return static_cast<int32>(GetAntiAliasingMethod()); }
	void SetCBlindModeInt(const int32 InValue) { SetColorBlindMode(static_cast<EGTColorBlindMode>(InValue)); }
	int32 GetCBlindModeInt() const { return static_cast<int32>(GetColorBlindMode()); }

	static TArray<FIntPoint> GetAllResolutions();

protected:

	UPROPERTY(Config) float Gamma;
	UPROPERTY(Config) EGTColorBlindMode ColorBlindMode;
	UPROPERTY(Config) uint8 ColorBlindStrength;
	UPROPERTY(Config) uint8 MotionBlurAmount;
	UPROPERTY(Config) EGTAntiAliasingMethod AntiAliasingMethod;

	void ApplyColorBlindSettings();
	virtual void SetToDefaults() override;
	virtual void ApplyNonResolutionSettings() override;

public: // Statics

	static int32 AntiAliasingMethodAsInt(const EGTAntiAliasingMethod InMethod)
	{
		switch (InMethod)
		{
		case EGTAntiAliasingMethod::FXAA: return 1;
		case EGTAntiAliasingMethod::TAA: return 2;
		case EGTAntiAliasingMethod::TSR: return 4;
		default: return 2;
		}
	}
};
