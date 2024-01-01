// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst

#include "GCSettingsWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"
#include "UserInterface/GCSubWidgets.h"
#include "UserSettings/GCUserSettings.h"
#include "Framework/GCGameInstance.h"

#define __SETUP_BASE(type, Property, GetterFunc, SetterFunc) \
	OnRefreshDisplay.AddUObject(Property, &UGCSettingRowBase::RefreshValue); \
	Property->AssignGetterFunction([]()->type{ return UGCUserSettings::Get()->GetterFunc(); }); \
	Property->AssignSetterFunction([](const type Value){ UGCUserSettings::Get()->SetterFunc(Value); })

#define SETUP_TOGGLE(Property, GetterFunc, SetterFunc) __SETUP_BASE(bool, Property, GetterFunc, SetterFunc)
#define SETUP_SWITCHER(Property, GetterFunc, SetterFunc) __SETUP_BASE(int32, Property, GetterFunc, SetterFunc)

#define SETUP_SLIDER(Property, BroadcastAllChanges, GetterFunc, SetterFunc) \
	__SETUP_BASE(float, Property, GetterFunc, SetterFunc); \
	Property->bBroadcastAnyChange = BroadcastAllChanges

#define SETUP_QUALITY_SWITCHER(Property, GetterFunc, SetterFunc) \
	SETUP_SWITCHER(Property, GetterFunc, SetterFunc); \
	Property->OnValueChanged.AddUObject(this, &UGCSettingsWidget::OnAnyScalabilityChanged)

#define SETUP_VOLUME_SLIDER(Property, Type) \
	OnRefreshDisplay.AddUObject(Property, &UGCSettingRowBase::RefreshValue); \
	Property->AssignGetterFunction([]()->float{ return UGCUserSettings::Get()->GetAudioVolume(EGCSoundType::Type); }); \
	Property->AssignSetterFunction([](const float Value){ UGCUserSettings::Get()->SetAudioVolume(EGCSoundType::Type, Value); })

#define __DEV_SETUP_BASE(type, Property, GetterFunc, SetterFunc) \
	OnRefreshDisplay.AddUObject(Property, &UGCSettingRowBase::RefreshValue); \
	Property->AssignGetterFunction([this]()->type{ return GetWorld()->GetGameInstance<UGCGameInstance>()->GetterFunc(); }); \
	Property->AssignSetterFunction([this](const type Value){ GetWorld()->GetGameInstance<UGCGameInstance>()->SetterFunc(Value); })

#define SETUP_DEV_TOGGLE(Property, GetterFunc, SetterFunc) __DEV_SETUP_BASE(bool, Property, GetterFunc, SetterFunc)
#define SETUP_DEV_SLIDER(Property, GetterFunc, SetterFunc) __DEV_SETUP_BASE(float, Property, GetterFunc, SetterFunc)
#define SETUP_DEV_SWITCHER(Property, GetterFunc, SetterFunc) __DEV_SETUP_BASE(int32, Property, GetterFunc, SetterFunc)

UGCSettingsWidget::UGCSettingsWidget(const FObjectInitializer& ObjectInitializer)
	: UGCUserWidget(ObjectInitializer)
{
	WidgetZOrder = 13;
	bAutoAddToViewport = false;

	GeneralButton = nullptr;
	GraphicsButton = nullptr;
	AudioButton = nullptr;
	KeybindsButton = nullptr;
	DebuggingButton = nullptr;
	
	ToggleFramerateRow = nullptr;
	FieldOfViewRow = nullptr;
	CameraSmoothingRow = nullptr;
	XSensitivityRow = nullptr;
	YSensitivityRow = nullptr;
	XInvertRow = nullptr;
	YInvertRow = nullptr;
	
	ResolutionBox = nullptr;
	ResScaleRow = nullptr;
	VSyncRow = nullptr;
	FrameRateRow = nullptr;
	GammaRow = nullptr;
	BrightnessRow = nullptr;
	
	CBlindModeRow = nullptr;
	CBlindStrengthRow = nullptr;

	FancyBloomRow = nullptr;
	MirrorQualityRow = nullptr;
	MBlurAmountRow = nullptr;
	AAliasingTypeRow = nullptr;
	FSRQualityRow = nullptr;
	FSRFrameInterpRow = nullptr;
	
	AutoDetectText = nullptr;
	AutoDetectButton = nullptr;
	OverallQualityRow = nullptr;
	ViewDistQualityRow = nullptr;
	AAliasingQualityRow = nullptr;
	ShadowQualityRow = nullptr;
	GIlluminationQualityRow = nullptr;
	ReflectionQualityRow = nullptr;
	PostProcessQualityRow = nullptr;
	TextureQualityRow = nullptr;
	EffectsQualityRow = nullptr;
	FoliageQualityRow = nullptr;
	ShadingQualityRow = nullptr;
	
	MasterVolRow = nullptr;
	AmbienceVolRow = nullptr;
	MusicVolRow = nullptr;
	SoundFXVolRow = nullptr;

	InvincibilityRow = nullptr;
	ViewModeUnlitRow = nullptr;
	
	ConfirmResText = nullptr;
	ConfirmResButton = nullptr;
	RevertResButton = nullptr;
	ExitButton = nullptr;
	SwapScreenAnim = nullptr;
	ConfirmResAnim = nullptr;
	
	SettingsObject = nullptr;
	ScreenIndex = 0;
	Resolutions = {};
	LastConfirmedRes = 0;
	bIsFinalResolution = true;
	ResolutionWaitTime = 0.0f;
	AutoDetectWaitTime = 0.0f;
}

void UGCSettingsWidget::RefreshDisplay()
{
	OnRefreshDisplay.Broadcast();
	PlayAnimation(FadeAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.5f);
}

void UGCSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	OnRefreshDisplay.Broadcast();
}

void UGCSettingsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SettingsObject = UGCUserSettings::Get();
	SettingsObject->LoadSettings();

	GeneralButton->OnClicked.AddUObject(this, &UGCSettingsWidget::OnGeneralButtonClicked);
	GraphicsButton->OnClicked.AddUObject(this, &UGCSettingsWidget::OnGraphicsButtonClicked);
	AudioButton->OnClicked.AddUObject(this, &UGCSettingsWidget::OnAudioButtonClicked);
	KeybindsButton->OnClicked.AddUObject(this, &UGCSettingsWidget::OnKeybindsButtonClicked);
	DebuggingButton->OnClicked.AddUObject(this, &UGCSettingsWidget::OnDebuggingButtonClicked);
	
	/* General */
	SETUP_TOGGLE(ToggleFramerateRow, GetShowFrameRate, SetShowFrameRate);
	SETUP_SLIDER(FieldOfViewRow, true, GetFieldOfView, SetFieldOfView);
	SETUP_TOGGLE(CameraSmoothingRow, GetEnableCameraSmoothing, SetEnableCameraSmoothing);
	SETUP_SLIDER(XSensitivityRow, false, GetSensitivityX, SetSensitivityX);
	SETUP_SLIDER(YSensitivityRow, false, GetSensitivityY, SetSensitivityY);
	SETUP_TOGGLE(XInvertRow, GetInvertMouseX, SetInvertMouseX);
	SETUP_TOGGLE(YInvertRow, GetInvertMouseY, SetInvertMouseY);
	/* ~General */
	
	/* Graphics */
	// Video
	ResolutionBox->ClearOptions();
	Resolutions = UGCUserSettings::GetAllResolutions();
	for (int i = 0; i < Resolutions.Num(); i++)
	{
		ResolutionBox->AddOption(FString::Printf(TEXT("%d x %d%s"), Resolutions[i].X, Resolutions[i].Y,
			i == 0 ? TEXT(" (Fullscreen)") : TEXT("")));
	}
	ResolutionBox->SetSelectedIndex(0);
	ResolutionBox->OnSelectionChanged.AddDynamic(this, &UGCSettingsWidget::OnResolutionChanged);
	OnRefreshDisplay.AddUObject(this, &UGCSettingsWidget::OnUpdateResolution);
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
	SETUP_SWITCHER(MirrorQualityRow, GetMirrorQuality, SetMirrorQuality);
	SETUP_SWITCHER(MBlurAmountRow, GetMotionBlurAmount, SetMotionBlurAmount);
	SETUP_SWITCHER(AAliasingTypeRow, GetAAliasingInt, SetAAliasingInt);
	SETUP_SWITCHER(FSRQualityRow, GetFSRQuality, SetFSRQuality);
	SETUP_TOGGLE(FSRFrameInterpRow, GetEnableFSRFrameInterp, SetEnableFSRFrameInterp);
	// Advanced
	AutoDetectText->SetText(FText::FromString(TEXT("Run Hardware Benchmark")));
	AutoDetectButton->OnClicked.AddDynamic(this, &UGCSettingsWidget::OnAutoDetectClicked);
	OverallQualityRow->OnValueChanged.AddUObject(this, &UGCSettingsWidget::OnOverallQualityChanged);
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
	SETUP_VOLUME_SLIDER(MasterVolRow, Master);
	SETUP_VOLUME_SLIDER(AmbienceVolRow, Ambience);
	SETUP_VOLUME_SLIDER(MusicVolRow, Music);
	SETUP_VOLUME_SLIDER(SoundFXVolRow, SoundFX);
	/* ~Audio */

	/* Developer */
	SETUP_DEV_TOGGLE(InvincibilityRow, GetPlayerInvincible, SetPlayerInvincible);
	SETUP_DEV_TOGGLE(ViewModeUnlitRow, GetViewModeUnlit, SetViewModeUnlit);
	/* ~Developer */
	
	ConfirmResButton->OnClicked.AddDynamic(this, &UGCSettingsWidget::OnConfirmResClicked);
	RevertResButton->OnClicked.AddDynamic(this, &UGCSettingsWidget::OnRevertResClicked);
	ExitButton->OnClicked.AddDynamic(this, &UGCSettingsWidget::OnExitClicked);
	
	OnRefreshDisplay.Broadcast();
}

void UGCSettingsWidget::NativeTick(const FGeometry& InGeometry, float DeltaTime)
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

void UGCSettingsWidget::SetScreenIndex(const int32 InIndex)
{
	if (ScreenIndex != InIndex && !IsAnimationPlaying(SwapScreenAnim))
	{
		ScreenIndex = InIndex;
		PlayAnimation(SwapScreenAnim);
	}
}

void UGCSettingsWidget::OnUpdateResolution()
{
	const int32 Idx = Resolutions.Find(SettingsObject->GetScreenResolution());
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

void UGCSettingsWidget::OnOverallQualityChanged(int32 Index, FName Value)
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

void UGCSettingsWidget::OnAnyScalabilityChanged(int32 Index, FName Value)
{
	OverallQualityRow->RefreshValue();
}

void UGCSettingsWidget::OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	const int32 Idx = ResolutionBox->GetSelectedIndex();
	if (SelectedItem == TEXT("Unknown") || !Resolutions.IsValidIndex(Idx))
	{
		return;
	}
	
#if WITH_EDITOR
	if (Idx == 0)
	{
		SettingsObject->SetFullscreenMode(GIsPlayInEditorWorld ? EWindowMode::WindowedFullscreen : EWindowMode::Fullscreen);
	}
	else
	{
		SettingsObject->SetFullscreenMode(EWindowMode::Windowed);
	}
#else
	SettingsObject->SetFullscreenMode(Idx == 0 ? EWindowMode::Fullscreen : EWindowMode::Windowed);
#endif

	SettingsObject->SetScreenResolution(Resolutions[Idx]); 
	SettingsObject->ApplyResolutionSettings(false);

	ResolutionBox->RemoveOption("Unknown");
	if (SelectionType != ESelectInfo::Direct)
	{
		PlayAnimation(ConfirmResAnim);
		bIsFinalResolution = false;
		ResolutionWaitTime = 10.0f;
	}
	else
	{
		LastConfirmedRes = ResolutionBox->GetSelectedIndex();
	}
}

void UGCSettingsWidget::OnAutoDetectClicked()
{
	AutoDetectWaitTime = 5.0f;
	AutoDetectButton->SetIsEnabled(false);

	SettingsObject->RunHardwareBenchmark(10, 1.0f, 1.0f);
	SettingsObject->ApplyHardwareBenchmarkResults();

	RefreshDisplay();
}

void UGCSettingsWidget::OnConfirmResClicked()
{
	bIsFinalResolution = true;
	ResolutionWaitTime = 0.0f;
	PlayAnimationReverse(ConfirmResAnim);
	ConfirmResText->SetText(FText::FromString(TEXT("Resolution Confirmed!")));
	LastConfirmedRes = ResolutionBox->GetSelectedIndex();
}

void UGCSettingsWidget::OnRevertResClicked()
{
	bIsFinalResolution = true;
	PlayAnimationReverse(ConfirmResAnim);
	ConfirmResText->SetText(FText::FromString(TEXT("Reverting Now!")));
	ResolutionBox->SetSelectedIndex(LastConfirmedRes);
}

void UGCSettingsWidget::OnExitClicked()
{
	SettingsObject->ApplySettings(false);
	
	RemoveWidget(nullptr, true);
	OnExit.Broadcast();
	OnExitBP.Broadcast();
}
