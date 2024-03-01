// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCUserSettings.h"
#include "Framework/GCGameInstance.h"
#include "Core/CoreLogging.h"
#include "Core/GCSettings.h"

#include "Rendering/RenderingCommon.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Framework/Application/SlateApplication.h"
#include "Internationalization/Culture.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "AudioDevice.h"

#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedActionKeyMapping.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystemInterface.h"
#include "JsonObjectWrapper.h"
#include "Libraries/RCCVarLibrary.h"

UGCUserSettings::UGCUserSettings()
{
	bInitializing = false;
	bLaunchWork = false;
	GameInstance = nullptr;
	SoundMixObject = nullptr;
	SoundTypeToClass = {};
	
	UGCUserSettings::SetToDefaults();
}

void UGCUserSettings::InitializeSettings()
{
	bInitializing = true;
	UE_LOG(UserSettings, Log, TEXT("Initializing User Settings with Game Instance %s"), *GetNameSafe(GameInstance));

	UGCSettings* Settings = UGCSettings::Get();
	SoundMixObject = Settings->SoundMixClass.LoadSynchronous();
	for (const EGCSoundType Type : TEnumRange<EGCSoundType>())
	{
		SoundTypeToClass.Add(Type, Settings->SoundClasses.FindOrAdd(Type).LoadSynchronous());
	}
	
	if (GameInstance->IsFirstLaunch() && !bLaunchWork)
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

void UGCUserSettings::SetOverallQuality(const int32 InValue)
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

int32 UGCUserSettings::GetOverallQuality() const
{
	const int32 Target = GetViewDistanceQuality();
	if ((Target == GetAntiAliasingQuality()) && (Target == GetShadowQuality()) && (Target == GetGlobalIlluminationQuality())
		&& (Target == GetReflectionQuality()) && (Target == GetPostProcessingQuality()) && (Target == GetTextureQuality())
		&& (Target == GetVisualEffectQuality()) && (Target == GetFoliageQuality()) && (Target == GetShadingQuality()))
	{
		return Target + 1;
	}

	return 0;
}

void UGCUserSettings::SetShowFrameRate(const bool bInShowFPS)
{
	if (bShowFramerate != bInShowFPS)
	{
		bShowFramerate = bInShowFPS;
		OnDynamicApply.Broadcast(this);
	}
}

void UGCUserSettings::SetFieldOfView(const int32 InFieldOfView)
{
	if (FieldOfView != InFieldOfView)
	{
		FieldOfView = InFieldOfView;
		OnDynamicApply.Broadcast(this);
	}
}

void UGCUserSettings::SetBrightness(const int32 InBrightness)
{
	if (Brightness != InBrightness)
	{
		Brightness = InBrightness;
		OnDynamicApply.Broadcast(this);
	}
}

void UGCUserSettings::SetGamma(const float InGamma)
{
	if (Gamma != InGamma)
	{
		Gamma = InGamma;
		GEngine->DisplayGamma = FMath::Clamp(InGamma, 0, 5);
	}
}

void UGCUserSettings::SetColorBlindMode(const EGCColorBlindMode InColorBlindMode)
{
	if (ColorBlindMode != InColorBlindMode)
	{
		ColorBlindMode = InColorBlindMode;
		ApplyColorBlindSettings();
	}
}

void UGCUserSettings::SetColorBlindStrength(const int32 InStrength)
{
	if (ColorBlindStrength != InStrength)
	{
		ColorBlindStrength = FMath::Clamp(InStrength, 0, 10);
		ApplyColorBlindSettings();
	}
}

void UGCUserSettings::SetUseFancyBloom(const bool bUseFancyBloom)
{
	bFancyBloom = bUseFancyBloom;
}

void UGCUserSettings::SetAntiAliasingMethod(const EGCAntiAliasingMethod InAntiAliasingMethod)
{
	AntiAliasingMethod = InAntiAliasingMethod;
}

void UGCUserSettings::SetMotionBlurAmount(const int32 InMotionBlurAmount)
{
	MotionBlurAmount = FMath::Clamp(InMotionBlurAmount, 0, 4);
}

void UGCUserSettings::SetFSRQuality(const int32 InFSRQuality)
{
	FSRQuality = FMath::Clamp(InFSRQuality, 0, 4);
}

void UGCUserSettings::SetEnableFSRFrameInterp(const bool bInEnableFSRFrameInterp)
{
	bFSRFrameInterp = bInEnableFSRFrameInterp;
}

void UGCUserSettings::SetEnableCameraSmoothing(const bool bInEnableCameraSmoothing)
{
	bEnableCameraSmoothing = bInEnableCameraSmoothing;
}

void UGCUserSettings::SetInvertMouseX(const bool bInInvertMouseX)
{
	bInvertMouseX = bInInvertMouseX;
}

void UGCUserSettings::SetInvertMouseY(const bool bInInvertMouseY)
{
	bInvertMouseY = bInInvertMouseY;
}

void UGCUserSettings::SetSensitivityX(const float InSensitivityX)
{
	SensitivityX = InSensitivityX;
}

void UGCUserSettings::SetSensitivityY(const float InSensitivityY)
{
	SensitivityY = InSensitivityY;
}

void UGCUserSettings::SetSensitivityXY(const FVector2D InSensitivityXY)
{
	SensitivityX = InSensitivityXY.X;
	SensitivityY = InSensitivityXY.Y;
}

void UGCUserSettings::UnmapPlayerKeyBinding(const FGCPlayerMappableKeyName InName)
{
	if (InName.IsValid())
	{
		KeyBindings.Remove(*InName);
	}
}

void UGCUserSettings::RemapPlayerKeyBinding(const FGCPlayerMappableKeyName InName, const FKey InNewKey)
{
	if (InName.IsValid() && InNewKey.IsValid())
	{
		KeyBindings.Add(*InName, InNewKey);
	}
}

bool UGCUserSettings::GetPlayerKeyBinding(const FGCPlayerMappableKeyName InName, FKey& OutKey) const
{
	if (!InName.IsValid() || !KeyBindings.Contains(*InName))
	{
		return false;
	}

	OutKey = KeyBindings.FindRef(*InName);
	return true;
}

void UGCUserSettings::SetAudioVolume(const EGCSoundType InSoundType, const int32 InNewVolume)
{
	SoundTypeToVolume.Add(InSoundType, InNewVolume);
	ApplyAudioSettings();
}

int32 UGCUserSettings::GetAudioVolume(const EGCSoundType InSoundType)
{
	return FMath::Abs(SoundTypeToVolume.FindOrAdd(InSoundType, 100));
}

TMap<EGCSoundType, int32> UGCUserSettings::GetAllAudioVolumes() const
{
	TMap<EGCSoundType, int32> Out;
	for (const TPair<EGCSoundType, uint8>& Pair : SoundTypeToVolume)
	{
		Out.Add(Pair.Key, Pair.Value);
	}

	return Out;
}

void UGCUserSettings::SetCulture(const FString& InCulture)
{
	if (Culture != InCulture)
	{
		FString PendingCulture = InCulture;
		if (PendingCulture.IsEmpty())
		{
			PendingCulture = FInternationalization::Get().GetDefaultCulture()->GetName();
		}
	
		TArray<FString> Cultures;
		FInternationalization::Get().GetCultureNames(Cultures);
		if (Cultures.Contains(PendingCulture))
		{
			Culture = PendingCulture;
			FInternationalization::Get().SetCurrentCulture(PendingCulture);
		}
	}
}

TArray<FString> UGCUserSettings::GetAllCultures()
{
	TArray<FString> Cultures;
	FInternationalization::Get().GetCultureNames(Cultures);
	return Cultures;
}

TArray<FIntPoint> UGCUserSettings::GetAllResolutions()
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

void UGCUserSettings::SetToDefaults()
{
	Super::SetToDefaults();
	
	SetScreenResolution(GetDesktopResolution());
#if WITH_EDITOR
	SetFullscreenMode(GIsPlayInEditorWorld ? EWindowMode::WindowedFullscreen : EWindowMode::Fullscreen);
#else
	SetFullscreenMode(EWindowMode::Fullscreen);
#endif
	SetOverallQuality(2);
	SetResScalePercent(100.0f);
	SetVSyncEnabled(true);
	FrameRateLimit = 60.0f;
	Culture = FInternationalization::Get().GetDefaultCulture()->GetName();
	bShowFramerate = false;
	FieldOfView = 90;
	Brightness = 100;
	Gamma = 2.2f;
	ColorBlindMode = EGCColorBlindMode::None;
	ColorBlindStrength = 10;
	bFancyBloom = true;
	AntiAliasingMethod = EGCAntiAliasingMethod::TAA;
	MotionBlurAmount = 1;
	FSRQuality = 0;
	bFSRFrameInterp = true;
	bEnableCameraSmoothing = true;
	bInvertMouseX = false;
	bInvertMouseY = false;
	SensitivityX = 1.0f;
	SensitivityY = 1.0f;
	KeyBindings = {};
	for (const EGCSoundType Type : TEnumRange<EGCSoundType>())
	{
		SoundTypeToVolume.Add(Type, 100);
	}

	if (FApp::IsGame() && !bInitializing)
	{
		if (GEngine)
			GEngine->DisplayGamma = 2.2;
		
		OnDynamicApply.Broadcast(this);
	}
}

void UGCUserSettings::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();

#if WITH_EDITOR
	if (!FApp::IsGame())
	{
		GEngine->DisplayGamma = 2.2;
		
		if (IConsoleVariable* CVar_MotionBlurQuality = URCCVarLibrary::FindCVar(TEXT("r.MotionBlurQuality")))
		{
			CVar_MotionBlurQuality->Set(2, ECVF_SetByConsole);
		}
		if (IConsoleVariable* CVar_AntiAliasingMethod = URCCVarLibrary::FindCVar(TEXT("r.AntiAliasingMethod")))
		{
			CVar_AntiAliasingMethod->Set(2, ECVF_SetByConsole);
		}
	}
	else
#endif
	{
		GEngine->DisplayGamma = FMath::Clamp(Gamma, 0, 5);
		
		if (IConsoleVariable* CVar_MotionBlurQuality = URCCVarLibrary::FindCVar(TEXT("r.MotionBlurQuality")))
		{
			CVar_MotionBlurQuality->Set(FMath::Clamp((int32)MotionBlurAmount, 0, 4), ECVF_SetByConsole);
		}
		if (IConsoleVariable* CVar_AntiAliasingMethod = URCCVarLibrary::FindCVar(TEXT("r.AntiAliasingMethod")))
		{
			CVar_AntiAliasingMethod->Set(ConvertAAMethod(AntiAliasingMethod), ECVF_SetByConsole);
		}
	}

	if (IConsoleVariable* CVar_FSRQuality = URCCVarLibrary::FindCVar(TEXT("r.FidelityFX.FSR3.QualityMode")))
	{
		CVar_FSRQuality->Set(FMath::Clamp((int32)FSRQuality, 0, 4), ECVF_SetByConsole);
	}
	if (IConsoleVariable* CVar_FSRFrameInterp = URCCVarLibrary::FindCVar(TEXT("r.AntiAliasingMethod")))
	{
		CVar_FSRFrameInterp->Set(bFSRFrameInterp ? 1 : 0, ECVF_SetByConsole);
	}

	ApplyColorBlindSettings();
	ApplyPlayerKeybinds();
	ApplyAudioSettings();
	
	OnManualApply.Broadcast(this);
	OnManualApplyBP.Broadcast(this);

	if (bInitializing)
	{
		OnDynamicApply.Broadcast(this);
	}

	SaveSettings();
}

int32 UGCUserSettings::ConvertAAMethod(const EGCAntiAliasingMethod InMethod)
{
	switch (InMethod)
	{
	case EGCAntiAliasingMethod::FXAA: return 1;
	case EGCAntiAliasingMethod::TAA: return 2;
	case EGCAntiAliasingMethod::TSR: return 4;
	default: return 2;
	}
}

float UGCUserSettings::IntToFrameRate(const int32 InValue)
{
	switch (InValue)
	{
	case 0: return 30.0f;
	case 1: return 60.0f;
	case 2: return 120.0f;
	default: return 0.0f;
	}
}

int32 UGCUserSettings::FrameRateToInt(const float InFramerate)
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

UWorld* UGCUserSettings::TryGetWorld() const
{
	if (GameInstance && GameInstance->GetWorld())
		return GameInstance->GetWorld();

	return GEngine ? GEngine->GetCurrentPlayWorld() : GWorld;
}

void UGCUserSettings::ApplyColorBlindSettings()
{
	if (!FApp::IsGame())
		return;
	
	FSlateApplication::Get().GetRenderer()->SetColorVisionDeficiencyType( static_cast<EColorVisionDeficiency>(ColorBlindMode),
		FMath::Clamp(ColorBlindStrength, 0, 10), true, false);
}

void UGCUserSettings::ApplyPlayerKeybinds()
{
	//if (!FApp::IsGame())
		return;

	const UWorld* World = TryGetWorld();
	if (!World)
	{
		UE_LOG(UserSettings, Error, TEXT("Failed to apply keybind settings. World is null."));
		return;
	}

	const APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(UserSettings, Error, TEXT("Failed to apply keybind settings. Player Controller is null."));
		return;
	}

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	if (!LP)
	{
		UE_LOG(UserSettings, Error, TEXT("Failed to apply keybind settings. Local Player is null."));
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSubsystem)
	{
		UE_LOG(UserSettings, Error, TEXT("Failed to apply keybind settings. Input Subsystem is null."));
		return;
	}

	//InputSubsystem->RemoveAllPlayerMappedKeys();
		
	const TArray<FEnhancedActionKeyMapping> Mappings = InputSubsystem->GetAllPlayerMappableActionKeyMappings();
	for (const FEnhancedActionKeyMapping& Mapping : Mappings)
	{
		const FKey* Key = KeyBindings.Find(Mapping.GetMappingName());
		if (!Key || !Key->IsValid())
			continue;

		//InputSubsystem->AddPlayerMappedKeyInSlot(Mapping.GetMappingName(), *Key);
	}
}

void UGCUserSettings::ApplyAudioSettings()
{
	if (!FApp::IsGame())
		return;

	if (!SoundMixObject)
	{
		UE_LOG(UserSettings, Error, TEXT("Failed to apply audio settings. Sound Mix is null."));
		return;
	}

	const UWorld* World = TryGetWorld();
	if (!World)
	{
		UE_LOG(UserSettings, Error, TEXT("Failed to apply audio settings. World is null."));
		return;
	}

	FAudioDeviceHandle AudioDevice = World->GetAudioDevice();
	if (!AudioDevice)
	{
		UE_LOG(UserSettings, Error, TEXT("Failed to apply audio settings. Audio Device is null."));
		return;
	}
	
	for (const EGCSoundType Type : TEnumRange<EGCSoundType>())
	{
		USoundClass* SoundClass = SoundTypeToClass.FindOrAdd(Type);
		if (!SoundClass)
		{
			UE_LOG(UserSettings, Error, TEXT("No sound class found for sound type %s."),
				*UEnum::GetDisplayValueAsText(Type).ToString());
			
			continue;
		}
		
		AudioDevice->SetSoundMixClassOverride(SoundMixObject, SoundClass, GetAudioVolume(Type) * 0.01f, 1.0f,
			0.5f, true);
	}

	AudioDevice->PushSoundMixModifier(SoundMixObject);
}
