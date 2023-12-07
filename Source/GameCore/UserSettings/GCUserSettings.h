// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "Engine/Engine.h"
#include "GCUserSettingTypes.h"
#include "GameFramework/GameUserSettings.h"
#include "GCUserSettings.generated.h"

class UEnhancedInputLocalPlayerSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSettingsApplyBP, class UGCUserSettings*, Settings);

UCLASS(BlueprintType, DisplayName = "User Settings Manager")
class GAMECORE_API UGCUserSettings final : public UGameUserSettings
{
	GENERATED_BODY()

	friend class UGCGameInstance;

public:
	
	UGCUserSettings();

	static UGCUserSettings* Get() { return Cast<UGCUserSettings>(GEngine->GameUserSettings); }
	
	UPROPERTY(BlueprintAssignable, DisplayName = "On Manual Apply")
		FOnSettingsApplyBP OnManualApplyBP;

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void InitializeSettings();
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetOverallQuality(const int32 InValue);

	UFUNCTION(BlueprintPure, Category = "Settings")
		int32 GetOverallQuality() const;
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetShowFrameRate(const bool bInShowFPS);

	UFUNCTION(BlueprintPure, Category = "Settings")
		FORCEINLINE bool GetShowFrameRate() const { return bShowFramerate; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetFieldOfView(const int32 InFieldOfView = 90);

	UFUNCTION(BlueprintPure, Category = "Settings")
		FORCEINLINE int32 GetFieldOfView() const { return FieldOfView; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetBrightness(const int32 InBrightness = 100);

	UFUNCTION(BlueprintPure, Category = "Settings")
		FORCEINLINE int32 GetBrightness() const { return Brightness; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetGamma(const float InGamma = 2.2f);

	UFUNCTION(BlueprintPure, Category = "Settings")
		FORCEINLINE float GetGamma() const { return Gamma; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetColorBlindMode(const EGCColorBlindMode InColorBlindMode);

	UFUNCTION(BlueprintPure, Category = "Settings")
		FORCEINLINE EGCColorBlindMode GetColorBlindMode() const { return ColorBlindMode; }
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetColorBlindStrength(const int32 InStrength = 10);

	UFUNCTION(BlueprintPure, Category = "Settings")
		FORCEINLINE int32 GetColorBlindStrength() const { return ColorBlindStrength; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetUseFancyBloom(const bool bUseFancyBloom = true);

	UFUNCTION(BlueprintPure, Category = "Settings")
		FORCEINLINE bool GetUseFancyBloom() const { return bFancyBloom; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetMirrorQuality(const int32 InMirrorQuality = 75);

	UFUNCTION(BlueprintPure, Category = "Settings")
		FORCEINLINE int32 GetMirrorQuality() const { return MirrorQuality; }
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetAntiAliasingMethod(const EGCAntiAliasingMethod InAntiAliasingMethod);

	UFUNCTION(BlueprintPure, Category = "Settings")
		FORCEINLINE EGCAntiAliasingMethod GetAntiAliasingMethod() const { return AntiAliasingMethod; }
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetMotionBlurAmount(const int32 InMotionBlurAmount = 2);

	UFUNCTION(BlueprintPure, Category = "Settings")
		FORCEINLINE int32 GetMotionBlurAmount() const { return MotionBlurAmount; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetEnableCameraSmoothing(const bool bInEnableCameraSmoothing);

	UFUNCTION(BlueprintPure, Category = "Settings")
		FORCEINLINE bool GetEnableCameraSmoothing() const { return bEnableCameraSmoothing; }
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetInvertMouseX(const bool bInInvertMouseX);

	UFUNCTION(BlueprintPure, Category = "Settings")
		FORCEINLINE bool GetInvertMouseX() const { return bInvertMouseX; }
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetInvertMouseY(const bool bInInvertMouseY);

	UFUNCTION(BlueprintPure, Category = "Settings")
		FORCEINLINE bool GetInvertMouseY() const { return bInvertMouseY; }
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetSensitivityX(const float InSensitivityX = 1.0f);

	UFUNCTION(BlueprintPure, Category = "Settings")
		FORCEINLINE float GetSensitivityX() const { return SensitivityX; }
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetSensitivityY(const float InSensitivityY = 1.0f);

	UFUNCTION(BlueprintPure, Category = "Settings")
		FORCEINLINE float GetSensitivityY() const { return SensitivityY; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetSensitivityXY(const FVector2D InSensitivityXY = FVector2D::UnitVector);
	
	UFUNCTION(BlueprintPure, Category = "Settings")
		FORCEINLINE FVector2D GetSensitivityXY() const { return FVector2D(SensitivityX, SensitivityY); }
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
		void UnmapPlayerKeyBinding(const FGCPlayerMappableKeyName InName);
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
		void RemapPlayerKeyBinding(const FGCPlayerMappableKeyName InName, const FKey InNewKey);
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
		bool GetPlayerKeyBinding(const FGCPlayerMappableKeyName InName, FKey& OutKey) const;

	UFUNCTION(BlueprintPure, Category = "Settings")
		FORCEINLINE TMap<FName, FKey> GetAllPlayerKeyBindings() const { return KeyBindings; }
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetAudioVolume(const EGCSoundType InSoundType, const int32 InNewVolume = 100);

	UFUNCTION(BlueprintPure, Category = "Settings")
		int32 GetAudioVolume(const EGCSoundType InSoundType);

	UFUNCTION(BlueprintPure, Category = "Settings")
		TMap<EGCSoundType, int32> GetAllAudioVolumes() const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetCulture(const FString& InCulture);

	UFUNCTION(BlueprintPure, Category = "Settings")
		FORCEINLINE FString GetCulture() const { return Culture; }
	
	UFUNCTION(BlueprintPure, Category = "Settings")
		static TArray<FString> GetAllCultures();
	
	UFUNCTION(BlueprintPure, Category = "Settings")
		static TArray<FIntPoint> GetAllResolutions();
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSettingsApply, UGCUserSettings*);
	FOnSettingsApply OnDynamicApply;
	FOnSettingsApply OnManualApply;

	virtual void SetToDefaults() override;
	virtual void ApplyNonResolutionSettings() override;
	
	FORCEINLINE void SetResScalePercent(const float InValue) { SetResolutionScaleNormalized(InValue * 0.01); }
	FORCEINLINE float GetResScalePercent() const { return GetResolutionScaleNormalized() * 100.0f; }
	FORCEINLINE void SetFrameRateInt(const int32 InValue) { SetFrameRateLimit(IntToFrameRate(InValue)); }
	FORCEINLINE int32 GetFrameRateInt() const { return FrameRateToInt(GetFrameRateLimit()); }
	FORCEINLINE void SetAAliasingInt(const int32 InValue) { SetAntiAliasingMethod(static_cast<EGCAntiAliasingMethod>(InValue)); }
	FORCEINLINE int32 GetAAliasingInt() const { return static_cast<int32>(GetAntiAliasingMethod()); }
	FORCEINLINE void SetCBlindModeInt(const int32 InValue) { SetColorBlindMode(static_cast<EGCColorBlindMode>(InValue)); }
	FORCEINLINE int32 GetCBlindModeInt() const { return static_cast<int32>(GetColorBlindMode()); }
	
	static int32 ConvertAAMethod(const EGCAntiAliasingMethod InMethod);
	static float IntToFrameRate(const int32 InValue);
	static int32 FrameRateToInt(const float InFramerate);

private:
	
	UWorld* TryGetWorld() const;
	
	void ApplyColorBlindSettings();
	void ApplyPlayerKeybinds();
	void ApplyAudioSettings();

	bool bInitializing;
	
	UPROPERTY(Transient)
		UGCGameInstance* GameInstance;

	UPROPERTY(Transient)
		class USoundMix* SoundMixObject;

	UPROPERTY(Transient)
		TMap<EGCSoundType, USoundClass*> SoundTypeToClass;
	
	UPROPERTY(Config)
		bool bShowFramerate;

	UPROPERTY(Config)
		uint8 FieldOfView;

	UPROPERTY(Config)
		uint8 Brightness;

	UPROPERTY(Config)
		float Gamma;

	UPROPERTY(Config)
		EGCColorBlindMode ColorBlindMode;

	UPROPERTY(Config)
		uint8 ColorBlindStrength;

	UPROPERTY(Config)
		bool bFancyBloom;

	UPROPERTY(Config)
		uint8 MirrorQuality;
	
	UPROPERTY(Config)
		EGCAntiAliasingMethod AntiAliasingMethod;

	UPROPERTY(Config)
		uint8 MotionBlurAmount;

	UPROPERTY(Config)
		bool bEnableCameraSmoothing;
	
	UPROPERTY(Config)
		bool bInvertMouseX;

	UPROPERTY(Config)
		bool bInvertMouseY;

	UPROPERTY(Config)
		float SensitivityX;

	UPROPERTY(Config)
		float SensitivityY;

	UPROPERTY(Config)
		TMap<FName, FKey> KeyBindings;

	UPROPERTY(Config)
		TMap<EGCSoundType, uint8> SoundTypeToVolume;

	UPROPERTY(Config)
		FString Culture;
};
