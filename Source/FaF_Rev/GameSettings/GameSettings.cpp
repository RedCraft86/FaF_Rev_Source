// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameSettings.h"
#include "AudioDevice.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "GTConfigSubsystem.h"
#include "FRGameInstance.h"
#include "FRSettings.h"

UGameSettings::UGameSettings()
	: bInitializing(false), bLaunchWork(false), GameInstance(nullptr), SoundMixObject(nullptr)
	, BrightnessParamName(NAME_None), BrightnessMPC(nullptr)
	, ScalabilityDefaults({3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2})
{
	UGameSettings::SetToDefaults();
}

void UGameSettings::InitializeSettings()
{
	bInitializing = true;

	const UFRSettings* Settings = FRSettings;
	BrightnessParamName = Settings->BrightnessParamName;
	BrightnessMPC = Settings->MainRenderingMPC.LoadSynchronous();
	SoundMixObject = Settings->SoundMixClass.LoadSynchronous();
	for (const EFRSoundType Type : TEnumRange<EFRSoundType>())
	{
		if (Settings->SoundClasses.FindRef(Type).IsNull()) continue;
		SoundTypeToClass.Add(Type, Settings->SoundClasses.FindRef(Type).LoadSynchronous());
	}

	//SetToDefaults();
	if (UGTConfigSubsystem::Get(GameInstance)->IsFirstLaunch() && !bLaunchWork)
	{
		bLaunchWork = true;
		RunHardwareBenchmark();
		ApplyHardwareBenchmarkResults();
		if (GetOverallQuality() > 3)
		{
			SetOverallQuality(3);
		}
		
		SetResolutionScaleNormalized(1.0f);
		CacheScalabilityDefaults();
	}
	else
	{
		LoadSettings(true);
	}
	
	ApplyNonResolutionSettings();
	bInitializing = false;
}

void UGameSettings::SetOverallQuality(const int32 InValue)
{
	if (InValue > 0)
	{
		SetViewDistanceQuality(InValue - 1);
		SetAntiAliasingQuality(InValue - 1);
		SetShadowQuality(InValue - 1);
		SetGlobalIlluminationQuality(InValue - 1);
		SetReflectionQuality(InValue - 1);
		SetPostProcessingQuality(InValue - 1);
		SetTextureQuality(InValue - 1);
		SetVisualEffectQuality(InValue - 1);
		SetFoliageQuality(InValue - 1);
		SetShadingQuality(InValue - 1);
	}
}

int32 UGameSettings::GetOverallQuality() const
{
	const int32 Target = GetViewDistanceQuality();
	if (Target == GetAntiAliasingQuality()
		&& Target == GetShadowQuality()
		&& Target == GetGlobalIlluminationQuality()
		&& Target == GetReflectionQuality()
		&& Target == GetPostProcessingQuality()
		&& Target == GetTextureQuality()
		&& Target == GetVisualEffectQuality()
		&& Target == GetFoliageQuality()
		&& Target == GetShadingQuality())
	{
		return Target + 1;
	}

	return 0;
}

void UGameSettings::SetShowFPS(const bool bInShowFPS)
{
	if (bShowFPS != bInShowFPS)
	{
		bShowFPS = bInShowFPS;
		OnDynamicApply.Broadcast();
	}
}

void UGameSettings::SetUseSmoothCamera(const bool bUseSmoothCamera)
{
	bSmoothCamera = bUseSmoothCamera;
}

void UGameSettings::SetSensitivityX(const float SensitivityX)
{
	SensitivityXY.X = SensitivityX;
}

void UGameSettings::SetInvertSensitivityX(const bool bInvertX)
{
	bInvertSensitivityX = bInvertX;
}

void UGameSettings::SetSensitivityY(const float SensitivityY)
{
	SensitivityXY.Y = SensitivityY;
}

void UGameSettings::SetInvertSensitivityY(const bool bInvertY)
{
	bInvertSensitivityY = bInvertY;
}

void UGameSettings::SetFieldOfView(const uint8 InFieldOfView)
{
	if (FieldOfView != InFieldOfView)
	{
		FieldOfView = InFieldOfView;
		OnDynamicApply.Broadcast();
	}
}

void UGameSettings::SetBrightness(const uint8 InBrightness)
{
	if (Brightness != InBrightness)
	{
		Brightness = InBrightness;
		ApplyBrightness();
	}
}

void UGameSettings::SetUseFancyBloom(const bool bUseFancyBloom)
{
	if (bFancyBloom != bUseFancyBloom)
	{
		bFancyBloom = bUseFancyBloom;
		OnDynamicApply.Broadcast();
	}
}

void UGameSettings::SetAudioVolume(const EFRSoundType Type, const uint8 InVolume)
{
	SoundTypeToVolume.Add(Type, InVolume);
	ApplyAudioSettings();
}

UWorld* UGameSettings::GetWorld() const
{
	UWorld* World = Super::GetWorld();
	if (!World) World = GameInstance ? GameInstance->GetWorld() : nullptr;
	if (!World) World = GEngine ? GEngine->GetCurrentPlayWorld() : GWorld;
	return World;
}

void UGameSettings::CacheScalabilityDefaults()
{
	ScalabilityDefaults[0] = GetOverallQuality();
	ScalabilityDefaults[1] = GetViewDistanceQuality();
	ScalabilityDefaults[2] = GetAntiAliasingQuality();
	ScalabilityDefaults[3] = GetShadowQuality();
	ScalabilityDefaults[4] = GetGlobalIlluminationQuality();
	ScalabilityDefaults[5] = GetReflectionQuality();
	ScalabilityDefaults[6] = GetPostProcessingQuality();
	ScalabilityDefaults[7] = GetTextureQuality();
	ScalabilityDefaults[8] = GetVisualEffectQuality();
	ScalabilityDefaults[9] = GetFoliageQuality();
	ScalabilityDefaults[10] = GetShadingQuality();
}

void UGameSettings::ApplyBrightness() const
{
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), BrightnessMPC,
		BrightnessParamName, FMath::Max(0.1f, Brightness * 0.01f));
}

void UGameSettings::ApplyAudioSettings()
{
	if (!FApp::IsGame()) return;
	if (!SoundMixObject) return;
	
	const UWorld* World = GetWorld();
	if (!ensure(World)) return;
	
	FAudioDeviceHandle AudioDevice = World->GetAudioDevice();
	if (!AudioDevice) return;
	
	for (const EFRSoundType Type : TEnumRange<EFRSoundType>())
	{
		const TObjectPtr<USoundClass>* SoundClass = SoundTypeToClass.Find(Type);
		if (!SoundClass || !*SoundClass) continue;
		
		AudioDevice->SetSoundMixClassOverride(SoundMixObject, *SoundClass,
			GetAudioVolumeValue(Type), 1.0f, 0.5f, true);
	}

	AudioDevice->PushSoundMixModifier(SoundMixObject);
}

void UGameSettings::SetToDefaults()
{
	Super::SetToDefaults();
	SetOverallQuality(3);
	
	bShowFPS = false;
	bSmoothCamera = true;
	SensitivityXY = FVector2D::UnitVector;
	bInvertSensitivityX = false;
	bInvertSensitivityY = false;
	FieldOfView = 90;
	Brightness = 100;
	bFancyBloom = true;
	for (const EFRSoundType Type : TEnumRange<EFRSoundType>())
	{
		SoundTypeToVolume.Add(Type, 100);
	}

	if (FApp::IsGame() && !bInitializing)
	{
		if (GEngine) GEngine->DisplayGamma = 2.2;
		OnDynamicApply.Broadcast();
	}
}

void UGameSettings::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();
	// if (IConsoleVariable* CVar_FSRQuality = UGTConsoleLibrary::FindCVar(TEXT("r.FidelityFX.FSR3.QualityMode")))
	// {
	// 	CVar_FSRQuality->Set(FMath::Clamp((int32)FSRQuality, 0, 4), ECVF_SetByConsole);
	// }

	ApplyBrightness();
	ApplyAudioSettings();
	
	OnManualApply.Broadcast();
	if (bInitializing) OnDynamicApply.Broadcast();

	SaveSettings();
}

int32 UGameSettings::FrameRateToInt(const float InFramerate)
{
	if (InFramerate <= 0.0f) return 3;
	if (InFramerate < 35.0f)
	{
		return 0;
	}
	if (InFramerate < 65.0f)
	{
		return 1;
	}
	if (InFramerate < 125.0f)
	{
		return 2;
	}

	return 3;
}

float UGameSettings::IntToFrameRate(const int32 InValue)
{
	switch (InValue)
	{
	case 0: return 30.0f;
	case 1: return 60.0f;
	case 2: return 120.0f;
	default: return 500.0f;
	}
}
