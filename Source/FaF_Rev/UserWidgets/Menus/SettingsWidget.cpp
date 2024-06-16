// Copyright (C) RedCraft86. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "SettingsWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"
#include "GameSettings/GameSettings.h"
#include "GTConfigSubsystem.h"
#include "WidgetInterface.h"
#include "FRGameInstance.h"
#include "FRSettings.h"
#include "SubWidgets.h"
#if UE_BUILD_SHIPPING
#include "GTConfigSubsystem.h"
#endif

#define SteamAudioCfgSection TEXT("[/Script/SteamAudio.SteamAudioSettings]")

#define __SETUP_BASE(type, Property, GetterFunc, SetterFunc) \
	OnRefreshDisplay.AddUObject(Property, &UFRSettingRowBase::RefreshValue); \
	Property->AssignGetterFunction([]()->type{ return UGameSettings::Get()->GetterFunc(); }); \
	Property->AssignSetterFunction([](const type Value){ UGameSettings::Get()->SetterFunc(Value); })

#define SETUP_TOGGLE(Property, GetterFunc, SetterFunc) __SETUP_BASE(bool, Property, GetterFunc, SetterFunc)
#define SETUP_SWITCHER(Property, GetterFunc, SetterFunc) __SETUP_BASE(int32, Property, GetterFunc, SetterFunc)

#define SETUP_SLIDER(Property, BroadcastAllChanges, GetterFunc, SetterFunc) \
	__SETUP_BASE(float, Property, GetterFunc, SetterFunc); \
	Property->bBroadcastAnyChange = BroadcastAllChanges

#define SETUP_QUALITY_SWITCHER(Property, GetterFunc, SetterFunc) \
	SETUP_SWITCHER(Property, GetterFunc, SetterFunc); \
	Property->OnValueChanged.AddUObject(this, &USettingsWidgetBase::OnAnyScalabilityChanged)

#define SETUP_VOLUME_SLIDER(Property, Type) \
	OnRefreshDisplay.AddUObject(Property, &UFRSettingRowBase::RefreshValue); \
	Property->AssignGetterFunction([]()->float{ return UGameSettings::Get()->GetAudioVolume(EFRSoundType::Type); }); \
	Property->AssignSetterFunction([](const float Value){ UGameSettings::Get()->SetAudioVolume(EFRSoundType::Type, Value); })

#define __DEV_SETUP_BASE(type, Property, GetterFunc, SetterFunc) \
	OnRefreshDisplay.AddUObject(Property, &UFRSettingRowBase::RefreshValue); \
	Property->AssignGetterFunction([this]()->type{ return GetWorld()->GetGameInstance<UFRGameInstance>()->GetterFunc(); }); \
	Property->AssignSetterFunction([this](const type Value){ GetWorld()->GetGameInstance<UFRGameInstance>()->SetterFunc(Value); })

#define SETUP_DEV_TOGGLE(Property, GetterFunc, SetterFunc) __DEV_SETUP_BASE(bool, Property, GetterFunc, SetterFunc)
#define SETUP_DEV_SLIDER(Property, GetterFunc, SetterFunc) __DEV_SETUP_BASE(float, Property, GetterFunc, SetterFunc)
#define SETUP_DEV_SWITCHER(Property, GetterFunc, SetterFunc) __DEV_SETUP_BASE(int32, Property, GetterFunc, SetterFunc)

USettingsWidgetBase::USettingsWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), GeneralButton(nullptr), GraphicsButton(nullptr), AudioButton(nullptr)
	, KeybindsButton(nullptr), DeveloperButton(nullptr), ToggleFramerateRow(nullptr), FieldOfViewRow(nullptr)
	, CameraSmoothingRow(nullptr), XSensitivityRow(nullptr), YSensitivityRow(nullptr), XInvertRow(nullptr)
	, YInvertRow(nullptr), ResolutionBox(nullptr), ResScaleRow(nullptr), VSyncRow(nullptr), FrameRateRow(nullptr)
	, GammaRow(nullptr), BrightnessRow(nullptr), CBlindModeRow(nullptr), CBlindStrengthRow(nullptr), FancyBloomRow(nullptr)
	, MBlurAmountRow(nullptr), AAliasingTypeRow(nullptr), AutoDetectButton(nullptr), AutoDetectText(nullptr)
	, OverallQualityRow(nullptr), ViewDistQualityRow(nullptr), AAliasingQualityRow(nullptr), ShadowQualityRow(nullptr)
	, GIlluminationQualityRow(nullptr), ReflectionQualityRow(nullptr), PostProcessQualityRow(nullptr)
	, TextureQualityRow(nullptr), EffectsQualityRow(nullptr), FoliageQualityRow(nullptr), ShadingQualityRow(nullptr)
	, MasterVolRow(nullptr), MusicVolRow(nullptr), SoundFXVolRow(nullptr), InvincibilityRow(nullptr)
	, ViewModeUnlitRow(nullptr), ConfirmResText(nullptr), ConfirmResButton(nullptr), RevertResButton(nullptr)
	, ExitButton(nullptr), SwapScreenAnim(nullptr), ConfirmResAnim(nullptr), ParentWidget(nullptr), ScreenIndex(0)
	, SettingsObj(nullptr), LastConfirmedResIdx(0), bIsFinalResolution(true), ResolutionWaitTime(0.0f), AutoDetectWaitTime(0.0f)
{
	ZOrder = 96;
	bAutoAdd = false;
}

void USettingsWidgetBase::RefreshUI()
{
	OnRefreshDisplay.Broadcast();
	PlayAnimation(FadeAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.5f);

	DeveloperButton->SetVisibility(UGTConfigSubsystem::Get(this)->IsDeveloperMode()
		? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void USettingsWidgetBase::SetScreenIndex(const uint8 InIndex)
{
	if (ScreenIndex != InIndex && !IsAnimationPlaying(SwapScreenAnim))
	{
		ScreenIndex = InIndex;
		PlayAnimation(SwapScreenAnim);
	}
}

void USettingsWidgetBase::RefreshScalability()
{
	OverallQualityRow->DefaultValue = SettingsObj->ScalabilityDefaults[0];
	ViewDistQualityRow->DefaultValue = SettingsObj->ScalabilityDefaults[1];
	AAliasingQualityRow->DefaultValue = SettingsObj->ScalabilityDefaults[2];
	ShadowQualityRow->DefaultValue = SettingsObj->ScalabilityDefaults[3];
	GIlluminationQualityRow->DefaultValue = SettingsObj->ScalabilityDefaults[4];
	ReflectionQualityRow->DefaultValue = SettingsObj->ScalabilityDefaults[5];
	PostProcessQualityRow->DefaultValue = SettingsObj->ScalabilityDefaults[6];
	TextureQualityRow->DefaultValue = SettingsObj->ScalabilityDefaults[7];
	EffectsQualityRow->DefaultValue = SettingsObj->ScalabilityDefaults[8];
	FoliageQualityRow->DefaultValue = SettingsObj->ScalabilityDefaults[9];
	ShadingQualityRow->DefaultValue = SettingsObj->ScalabilityDefaults[10];
}

void USettingsWidgetBase::RefreshResolutions()
{
	ResolutionBox->OnSelectionChanged.RemoveAll(this);
	ResolutionBox->ClearOptions();
	Resolutions = UGameSettings::GetAllResolutions();
	for (int i = 0; i < Resolutions.Num(); i++)
	{
		ResolutionBox->AddOption(FString::Printf(TEXT("%d x %d%s"), Resolutions[i].X, Resolutions[i].Y,
			i == 0 ? TEXT(" (Fullscreen)") : TEXT("")));
	}
	ResolutionBox->OnSelectionChanged.AddDynamic(this, &USettingsWidgetBase::OnResolutionChanged);
	ResolutionBox->SetSelectedIndex(FMath::Max(0, Resolutions.Find(SettingsObj->GetScreenResolution())));
}

void USettingsWidgetBase::OnUpdateResolution()
{
	const int32 Idx = Resolutions.Find(SettingsObj->GetScreenResolution());
	ResolutionBox->RemoveOption("Unknown");
	if (Idx == INDEX_NONE)
	{
		ResolutionBox->AddOption(TEXT("Unknown"));
		ResolutionBox->SetSelectedOption(TEXT("Unknown"));
	}
	else
	{
		ResolutionBox->SetSelectedIndex(Idx);
	}
}

void USettingsWidgetBase::OnOverallQualityChanged(int32 Index, FName Value)
{
	ViewDistQualityRow->RefreshValue();
	AAliasingQualityRow->RefreshValue();
	ShadowQualityRow->RefreshValue();
	GIlluminationQualityRow->RefreshValue();
	ReflectionQualityRow->RefreshValue();
	PostProcessQualityRow->RefreshValue();
	TextureQualityRow->RefreshValue();
	EffectsQualityRow->RefreshValue();
	FoliageQualityRow->RefreshValue();
	ShadingQualityRow->RefreshValue();
}

void USettingsWidgetBase::OnAnyScalabilityChanged(int32 Index, FName Value)
{
	OverallQualityRow->RefreshValue();
}

void USettingsWidgetBase::OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	const int32 Idx = ResolutionBox->GetSelectedIndex();
	if (SelectedItem == TEXT("Unknown") || !Resolutions.IsValidIndex(Idx))
	{
		return;
	}
	
	SettingsObj->SetScreenResolution(Resolutions[Idx]);
	SettingsObj->SetFullscreenMode(Idx == 0 ?
#if WITH_EDITOR
		EWindowMode::WindowedFullscreen
#else
		EWindowMode::Fullscreen
#endif
		: EWindowMode::Windowed);
	SettingsObj->ApplyResolutionSettings(false);

	ResolutionBox->RemoveOption("Unknown");
	if (SelectionType != ESelectInfo::Direct)
	{
		PlayAnimation(ConfirmResAnim);
		bIsFinalResolution = false;
		ResolutionWaitTime = 10.0f;
	}
	else
	{
		LastConfirmedResIdx = ResolutionBox->GetSelectedIndex();
	}
}

void USettingsWidgetBase::OnAutoDetectClicked()
{
	AutoDetectWaitTime = 5.0f;
	AutoDetectButton->SetIsEnabled(false);

	SettingsObj->RunHardwareBenchmark(10, 1.0f, 1.0f);
	SettingsObj->ApplyHardwareBenchmarkResults();

	RefreshUI();
}

void USettingsWidgetBase::OnConfirmResClicked()
{
	bIsFinalResolution = true;
	ResolutionWaitTime = 0.0f;
	PlayAnimationReverse(ConfirmResAnim);
	ConfirmResText->SetText(FText::FromString(TEXT("Resolution Confirmed!")));
	LastConfirmedResIdx = ResolutionBox->GetSelectedIndex();
}

void USettingsWidgetBase::OnRevertResClicked()
{
	bIsFinalResolution = true;
	PlayAnimationReverse(ConfirmResAnim);
	ConfirmResText->SetText(FText::FromString(TEXT("Reverting Now!")));
	ResolutionBox->SetSelectedIndex(LastConfirmedResIdx);
}

void USettingsWidgetBase::OnRestartClicked()
{
	RemoveFromParent();
	GetPlayerController()->SetPause(false);
	GetWorld()->GetWorldSettings()->SetTimeDilation(0.00001f);
	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float)->bool
	{
		UKismetSystemLibrary::QuitGame(this, GetPlayerController(),
			EQuitPreference::Quit, false);
		return false;
	}), 0.5f);
}

void USettingsWidgetBase::OnExitClicked()
{
	SettingsObj->ApplySettings(false);

	if (RequiresRestart.IsEmpty())
	{
		RemoveWidget(nullptr);
		PlayAnimationReverse(RestartAnim, 50.0f);
		IWidgetInterface::Return(ParentWidget, this);
	}
	else
	{
		FString Constructed = TEXT("");
		int num = RequiresRestart.Num();
		for (const FString& Setting : RequiresRestart)
		{
			num--;
			Constructed += Setting;
			if (num <= 0) Constructed += TEXT(", ");
		}
		
		RestartGameText->SetText(FText::FromString(Constructed));
		PlayAnimation(RestartAnim);
		RequiresRestart.Empty();
	}
}

void USettingsWidgetBase::InitWidget()
{
	SettingsObj = UGameSettings::Get();
	SettingsObj->LoadSettings();

	GeneralButton->OnClicked.AddUObject(this, &USettingsWidgetBase::OnGeneralButtonClicked);
	GraphicsButton->OnClicked.AddUObject(this, &USettingsWidgetBase::OnGraphicsButtonClicked);
	AudioButton->OnClicked.AddUObject(this, &USettingsWidgetBase::OnAudioButtonClicked);
	KeybindsButton->OnClicked.AddUObject(this, &USettingsWidgetBase::OnKeybindsButtonClicked);
	DeveloperButton->OnClicked.AddUObject(this, &USettingsWidgetBase::OnDebuggingButtonClicked);

	/* General */
	SETUP_TOGGLE(ToggleFramerateRow, GetShowFPS, SetShowFPS);
	SETUP_SLIDER(FieldOfViewRow, true, GetFieldOfView, SetFieldOfView);
	SETUP_TOGGLE(CameraSmoothingRow, GetUseSmoothCamera, SetUseSmoothCamera);
	SETUP_SLIDER(XSensitivityRow, false, GetSensitivityX, SetSensitivityX);
	SETUP_SLIDER(YSensitivityRow, false, GetSensitivityY, SetSensitivityY);
	SETUP_TOGGLE(XInvertRow, GetInvertSensitivityX, SetInvertSensitivityX);
	SETUP_TOGGLE(YInvertRow, GetInvertSensitivityY, SetInvertSensitivityY);
	/* ~General */
	
	/* Graphics */
	// Video
	RefreshResolutions();
	OnRefreshDisplay.AddUObject(this, &USettingsWidgetBase::OnUpdateResolution);
	SETUP_SLIDER(ResScaleRow, true, GetResScalePercent, SetResScalePercent);
	SETUP_TOGGLE(VSyncRow, IsVSyncEnabled, SetVSyncEnabled);
	SETUP_SWITCHER(FrameRateRow, GetFrameRateInt, SetFrameRateInt);
	SETUP_SLIDER(GammaRow, true, GetGamma, SetGamma);
	SETUP_SLIDER(BrightnessRow, true, GetBrightness, SetBrightness);
	// Color Blind
	SETUP_SWITCHER(CBlindModeRow, GetCBlindModeInt, SetCBlindModeInt);
	SETUP_SLIDER(CBlindStrengthRow, true, GetColorBlindStrength, SetColorBlindStrength);
	//Rendering Features
	SETUP_TOGGLE(FancyBloomRow, GetUseFancyBloom, SetUseFancyBloom);
	SETUP_SWITCHER(MBlurAmountRow, GetMotionBlurAmount, SetMotionBlurAmount);
	SETUP_SWITCHER(AAliasingTypeRow, GetAAliasingInt, SetAAliasingInt);
	// SETUP_SWITCHER(FSRQualityRow, GetFSRQuality, SetFSRQuality);
	// SETUP_TOGGLE(FSRFrameInterpRow, GetEnableFSRFrameInterp, SetEnableFSRFrameInterp);
	// Advanced
	AutoDetectText->SetText(FText::FromString(TEXT("Run Hardware Benchmark")));
	AutoDetectButton->OnClicked.AddDynamic(this, &USettingsWidgetBase::OnAutoDetectClicked);
	OverallQualityRow->OnValueChanged.AddUObject(this, &USettingsWidgetBase::OnOverallQualityChanged);
	SETUP_SWITCHER(OverallQualityRow, GetOverallQuality, SetOverallQuality);
	SETUP_QUALITY_SWITCHER(ViewDistQualityRow, GetViewDistanceQuality, SetViewDistanceQuality);
	SETUP_QUALITY_SWITCHER(AAliasingQualityRow, GetAntiAliasingQuality, SetAntiAliasingQuality);
	SETUP_QUALITY_SWITCHER(ShadowQualityRow, GetShadowQuality, SetShadowQuality);
	SETUP_QUALITY_SWITCHER(GIlluminationQualityRow, GetGlobalIlluminationQuality, SetGlobalIlluminationQuality);
	SETUP_QUALITY_SWITCHER(ReflectionQualityRow, GetReflectionQuality, SetReflectionQuality);
	SETUP_QUALITY_SWITCHER(PostProcessQualityRow, GetPostProcessingQuality, SetPostProcessingQuality);
	SETUP_QUALITY_SWITCHER(TextureQualityRow, GetTextureQuality, SetTextureQuality);
	SETUP_QUALITY_SWITCHER(EffectsQualityRow, GetVisualEffectQuality, SetVisualEffectQuality);
	SETUP_QUALITY_SWITCHER(FoliageQualityRow, GetFoliageQuality, SetFoliageQuality);
	SETUP_QUALITY_SWITCHER(ShadingQualityRow, GetShadingQuality, SetShadingQuality);
	/* ~Graphics */

	/* Audio */
	// Volume
	SETUP_VOLUME_SLIDER(MasterVolRow, Master);
	SETUP_VOLUME_SLIDER(MusicVolRow, Music);
	SETUP_VOLUME_SLIDER(SoundFXVolRow, SoundFX);
	// Audio Engine
	/* ~Audio */

	/* Developer */
	SETUP_DEV_TOGGLE(InvincibilityRow, IsPlayerInvincible, SetPlayerInvincible);
	SETUP_DEV_TOGGLE(ViewModeUnlitRow, IsUnlitViewMode, SetUnlitViewMode);
	/* ~Developer */

	ConfirmResButton->OnClicked.AddDynamic(this, &USettingsWidgetBase::OnConfirmResClicked);
	RevertResButton->OnClicked.AddDynamic(this, &USettingsWidgetBase::OnRevertResClicked);
	RestartButton->OnClicked.AddDynamic(this, &USettingsWidgetBase::OnRestartClicked);
	ExitRestartButton->OnClicked.AddDynamic(this, &USettingsWidgetBase::OnExitClicked);
	ExitButton->OnClicked.AddDynamic(this, &USettingsWidgetBase::OnExitClicked);

	OnRefreshDisplay.Broadcast();
}

void USettingsWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshScalability();
	RefreshResolutions();
	RefreshUI();
}

void USettingsWidgetBase::NativeTick(const FGeometry& InGeometry, float DeltaTime)
{
	Super::NativeTick(InGeometry, DeltaTime);
	if (!bIsFinalResolution && ResolutionWaitTime > 0.0f)
	{
		ResolutionWaitTime -= DeltaTime;
		if (ResolutionWaitTime <= 0.0f)
		{
			OnRevertResClicked();
		}
		else
		{
			ConfirmResText->SetText(FText::FromString(FString::Printf(TEXT("Reverting in %d seconds..."),
				FMath::CeilToInt(ResolutionWaitTime))));
		}
	}

	if (AutoDetectWaitTime > 0.0f)
	{
		AutoDetectWaitTime -= DeltaTime;
		if (AutoDetectWaitTime <= 0.0f)
		{
			AutoDetectButton->SetIsEnabled(true);
			AutoDetectText->SetText(FText::FromString(TEXT("Run Hardware Benchmark")));
		}
		else
		{
			AutoDetectText->SetText(FText::FromString(FString::Printf(TEXT("Run Hardware Benchmark (%ds)"),
				FMath::CeilToInt(AutoDetectWaitTime))));
		}
	}
}
