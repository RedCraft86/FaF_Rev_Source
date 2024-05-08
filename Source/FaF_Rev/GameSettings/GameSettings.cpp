// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameSettings.h"
#include "AudioDevice.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Libraries/GTConsoleLibrary.h"
#include "GTConfigSubsystem.h"
#include "FRGameInstance.h"
#include "FRSettings.h"


UGameSettings::UGameSettings()
{
	bInitializing = false;
	bLaunchWork = false;
	GameInst = nullptr;
	SoundMixObject = nullptr;
	SoundTypeToClass = {};
	
	UGameSettings::SetToDefaults();
}

void UGameSettings::InitializeSettings()
{
	bInitializing = true;

	UFRSettings* Settings = FRSettings;
	SoundMixObject = Settings->SoundMixClass.LoadSynchronous();
	for (const EFRSoundType Type : TEnumRange<EFRSoundType>())
	{
		SoundTypeToClass.Add(Type, Settings->SoundClasses.FindOrAdd(Type).LoadSynchronous());
	}
	
	if (UGTConfigSubsystem::Get(GameInst)->IsFirstLaunch() && !bLaunchWork)
	{
		bLaunchWork = true;
#if !WITH_EDITOR
		this->SetToDefaults();
		this->SetOverallQuality(3);
		this->SetResScalePercent(100.0f);
#endif
	}
	else
	{
		SetToDefaults();
		LoadSettings(true);
	}
	
	ApplySettings(false);
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

void UGameSettings::SetSensitivityY(const float SensitivityY)
{
	SensitivityXY.Y = SensitivityY;
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
		OnDynamicApply.Broadcast();
	}
}

void UGameSettings::SetGamma(const float InGamma)
{
	if (Gamma != InGamma)
	{
		Gamma = FMath::Clamp(InGamma, 0, 5);
		if (GEngine) GEngine->DisplayGamma = InGamma;
	}
}

void UGameSettings::SetColorBlindMode(const EFRColorBlindMode InColorBlindMode)
{
	if (ColorBlindMode != InColorBlindMode)
	{
		ColorBlindMode = InColorBlindMode;
		ApplyColorBlindSettings();
	}
}

void UGameSettings::SetColorBlindStrength(const uint8 InColorBlindStrength)
{
	if (ColorBlindStrength != InColorBlindStrength)
	{
		ColorBlindStrength = FMath::Clamp(InColorBlindStrength, 0, 10);
		ApplyColorBlindSettings();
	}
}

void UGameSettings::SetUseFancyBloom(const float bUseFancyBloom)
{
	bFancyBloom = bUseFancyBloom;
}

void UGameSettings::SetMotionBlurAmount(const uint8 InMotionBlurAmount)
{
	MotionBlurAmount = FMath::Clamp(InMotionBlurAmount, 0, 4);
}

void UGameSettings::SetAntiAliasingMethod(const EFRAntiAliasingMethod InAntiAliasingMethod)
{
	AntiAliasingMethod = InAntiAliasingMethod;
}

void UGameSettings::SetAudioVolume(const EFRSoundType Type, const uint8 InVolume)
{
	SoundTypeToVolume.Add(Type, InVolume);
	ApplyAudioSettings();
}

TArray<FIntPoint> UGameSettings::GetAllResolutions() const
{
	TArray<FIntPoint> Supported;
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(Supported);
	
	FDisplayMetrics Metrics;
	FDisplayMetrics::RebuildDisplayMetrics(Metrics);

	TArray<FIntPoint> Result;
	for (const FIntPoint& Elem : Supported)
	{
		if (Elem.X <= Metrics.PrimaryDisplayWidth
			&& Elem.Y <= Metrics.PrimaryDisplayHeight)
		{
			Result.Add(Elem);
		}
	}
	
	Algo::Reverse(Result);
	return Result;
}

UWorld* UGameSettings::GetWorld() const
{
	UWorld* World = Super::GetWorld();
	if (!World) World = GameInst ? GameInst->GetWorld() : nullptr;
	if (!World) World = GEngine ? GEngine->GetCurrentPlayWorld() : GWorld;
	return World;
}

void UGameSettings::ApplyAudioSettings()
{
	if (!FApp::IsGame()) return;
	
	const UWorld* World = GetWorld();
	if (!ensure(World)) return;
	
	FAudioDeviceHandle AudioDevice = World->GetAudioDevice();
	for (const EFRSoundType Type : TEnumRange<EFRSoundType>())
	{
		USoundClass* SoundClass = SoundTypeToClass.FindOrAdd(Type);
		AudioDevice->SetSoundMixClassOverride(SoundMixObject, SoundClass,
			GetAudioVolumeValue(Type), 1.0f, 0.5f, true);
	}

	AudioDevice->PushSoundMixModifier(SoundMixObject);
}

void UGameSettings::ApplyColorBlindSettings()
{
	if (!FApp::IsGame()) return;
	FSlateApplication::Get().GetRenderer()->SetColorVisionDeficiencyType(static_cast<EColorVisionDeficiency>(ColorBlindMode),
		FMath::Clamp(ColorBlindStrength, 0, 10), true, false);
}

void UGameSettings::SetToDefaults()
{
	Super::SetToDefaults();
	
	SetScreenResolution(GetDesktopResolution());
#if WITH_EDITOR
	SetFullscreenMode(GIsPlayInEditorWorld ? EWindowMode::WindowedFullscreen : EWindowMode::Fullscreen);
#else
	SetFullscreenMode(EWindowMode::Fullscreen);
#endif
	SetOverallQuality(2);
	SetResolutionScaleNormalized(1.0f);
	SetVSyncEnabled(true);
	FrameRateLimit = 60.0f;
	
	bShowFPS = false;
	bSmoothCamera = true;
	SensitivityXY = FVector2D::UnitVector;
	FieldOfView = 90;
	Brightness = 100;
	Gamma = 2.2f;
	ColorBlindMode = EFRColorBlindMode::None;
	ColorBlindStrength = 10;
	bFancyBloom = true;
	MotionBlurAmount = 1;
	AntiAliasingMethod = EFRAntiAliasingMethod::TAA;
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
#if WITH_EDITOR
	if (!FApp::IsGame())
	{
		GEngine->DisplayGamma = 2.2;
		
		if (IConsoleVariable* CVar_MotionBlurQuality = UGTConsoleLibrary::FindCVar(TEXT("r.MotionBlurQuality")))
		{
			CVar_MotionBlurQuality->Set(2, ECVF_SetByConsole);
		}
		if (IConsoleVariable* CVar_AntiAliasingMethod = UGTConsoleLibrary::FindCVar(TEXT("r.AntiAliasingMethod")))
		{
			CVar_AntiAliasingMethod->Set(2, ECVF_SetByConsole);
		}
	}
	else
#endif
	{
		GEngine->DisplayGamma = FMath::Clamp(Gamma, 0, 5);
		
		if (IConsoleVariable* CVar_MotionBlurQuality = UGTConsoleLibrary::FindCVar(TEXT("r.MotionBlurQuality")))
		{
			CVar_MotionBlurQuality->Set(FMath::Clamp((int32)MotionBlurAmount, 0, 4), ECVF_SetByConsole);
		}
		if (IConsoleVariable* CVar_AntiAliasingMethod = UGTConsoleLibrary::FindCVar(TEXT("r.AntiAliasingMethod")))
		{
			CVar_AntiAliasingMethod->Set(ConvertAAMethod(AntiAliasingMethod), ECVF_SetByConsole);
		}
	}
	
	// if (IConsoleVariable* CVar_FSRQuality = UGTConsoleLibrary::FindCVar(TEXT("r.FidelityFX.FSR3.QualityMode")))
	// {
	// 	CVar_FSRQuality->Set(FMath::Clamp((int32)FSRQuality, 0, 4), ECVF_SetByConsole);
	// }
	
	ApplyAudioSettings();
	ApplyColorBlindSettings();
	
	OnManualApply.Broadcast();
	if (bInitializing) OnDynamicApply.Broadcast();

	SaveSettings();
}

int32 UGameSettings::ConvertAAMethod(const EFRAntiAliasingMethod InMethod)
{
	switch (InMethod)
	{
	case EFRAntiAliasingMethod::FXAA: return 1;
	case EFRAntiAliasingMethod::TAA: return 2;
	case EFRAntiAliasingMethod::TSR: return 4;
	default: return 2;
	}
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
	default: return 0.0f;
	}
}
