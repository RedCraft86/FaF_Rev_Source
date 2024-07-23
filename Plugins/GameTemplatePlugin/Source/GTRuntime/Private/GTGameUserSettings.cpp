// Copyright (C) RedCraft86. All Rights Reserved.

#include "GTGameUserSettings.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Libraries/GTConsoleLibrary.h"

void UGTGameUserSettings::SetGamma(const float InGamma)
{
	if (Gamma != InGamma)
	{
		Gamma = FMath::Clamp(InGamma, 0, 5);
		if (GEngine) GEngine->DisplayGamma = InGamma;
	}
}

void UGTGameUserSettings::SetColorBlindMode(const EGTColorBlindMode InColorBlindMode)
{
	if (ColorBlindMode != InColorBlindMode)
	{
		ColorBlindMode = InColorBlindMode;
		ApplyColorBlindSettings();
	}
}

void UGTGameUserSettings::SetColorBlindStrength(const uint8 InColorBlindStrength)
{
	if (ColorBlindStrength != InColorBlindStrength)
	{
		ColorBlindStrength = FMath::Clamp(InColorBlindStrength, 0, 10);
		ApplyColorBlindSettings();
	}
}

void UGTGameUserSettings::SetMotionBlurAmount(const uint8 InMotionBlurAmount)
{
	MotionBlurAmount = FMath::Clamp(InMotionBlurAmount, 0, 4);
}

void UGTGameUserSettings::SetAntiAliasingMethod(const EGTAntiAliasingMethod InAntiAliasingMethod)
{
	AntiAliasingMethod = InAntiAliasingMethod;
}

TArray<FIntPoint> UGTGameUserSettings::GetAllResolutions()
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

void UGTGameUserSettings::ApplyColorBlindSettings()
{
	if (!FApp::IsGame()) return;
	FSlateApplication::Get().GetRenderer()->SetColorVisionDeficiencyType(static_cast<EColorVisionDeficiency>(ColorBlindMode),
		FMath::Clamp(ColorBlindStrength, 0, 10), true, false);
}

void UGTGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();
#if WITH_EDITOR
	SetScreenResolution({1920, 1080});
	SetFullscreenMode(EWindowMode::Windowed);
	PreferredFullscreenMode = EWindowMode::Windowed;
	LastUserConfirmedResolutionSizeX = 1920;
	LastUserConfirmedResolutionSizeY = 1080;
#else
	SetScreenResolution(GetDesktopResolution());
	SetFullscreenMode(EWindowMode::Fullscreen);
	PreferredFullscreenMode = EWindowMode::Windowed;
	LastUserConfirmedResolutionSizeX = ResolutionSizeX;
	LastUserConfirmedResolutionSizeY = ResolutionSizeY;
#endif
	SetResolutionScaleNormalized(1.0f);
	SetVSyncEnabled(true);
	FrameRateLimit = 60.0f;
	
	Gamma = 2.2f;
	ColorBlindMode = EGTColorBlindMode::None;
	ColorBlindStrength = 10;

	MotionBlurAmount = 1;
	AntiAliasingMethod = EGTAntiAliasingMethod::TAA;
}

void UGTGameUserSettings::ApplyNonResolutionSettings()
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
			CVar_AntiAliasingMethod->Set(AntiAliasingMethodAsInt(AntiAliasingMethod), ECVF_SetByConsole);
		}
	}
	
	ApplyColorBlindSettings();
}
