// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "SettingsWidget.generated.h"

class UFRToggleSettingBase;
class UFRSliderSettingBase;
class UFRSwitcherSettingBase;
class UFRAnimatedButtonBase;
class UButton;

UCLASS(Abstract)
class FAF_REV_API USettingsWidgetBase final : public UGTUserWidget
{
	GENERATED_BODY()

public:

	USettingsWidgetBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRAnimatedButtonBase> GeneralButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRAnimatedButtonBase> GraphicsButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRAnimatedButtonBase> AudioButton;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRAnimatedButtonBase> KeybindsButton;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRAnimatedButtonBase> DeveloperButton;
	
	/* General */
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRToggleSettingBase> ToggleFramerateRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSliderSettingBase> FieldOfViewRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRToggleSettingBase> CameraSmoothingRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSliderSettingBase> XSensitivityRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSliderSettingBase> YSensitivityRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRToggleSettingBase> XInvertRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRToggleSettingBase> YInvertRow;
	/* ~General */

	/* Graphics */
	// Video
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UComboBoxString> ResolutionBox;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSliderSettingBase> ResScaleRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRToggleSettingBase> VSyncRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSwitcherSettingBase> FrameRateRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSliderSettingBase> GammaRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSliderSettingBase> BrightnessRow;
	
	// Color Blind
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSwitcherSettingBase> CBlindModeRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSliderSettingBase> CBlindStrengthRow;
	
	// Rendering Features
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRToggleSettingBase> FancyBloomRow;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSwitcherSettingBase> MBlurAmountRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSwitcherSettingBase> AAliasingTypeRow;

	// UPROPERTY(Transient, meta = (BindWidget))
	// 	TObjectPtr<UFRSwitcherSettingBase> FSRQualityRow;
	//
	// UPROPERTY(Transient, meta = (BindWidget))
	// 	TObjectPtr<UFRToggleSettingBase> FSRFrameInterpRow;
	
	// Advanced
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> AutoDetectButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UTextBlock> AutoDetectText;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSwitcherSettingBase> OverallQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSwitcherSettingBase> ViewDistQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSwitcherSettingBase> AAliasingQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSwitcherSettingBase> ShadowQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSwitcherSettingBase> GIlluminationQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSwitcherSettingBase> ReflectionQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSwitcherSettingBase> PostProcessQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSwitcherSettingBase> TextureQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSwitcherSettingBase> EffectsQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSwitcherSettingBase> FoliageQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSwitcherSettingBase> ShadingQualityRow;
	/* ~Graphics */

	/* Audio */
	// Volume
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSliderSettingBase> MasterVolRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSliderSettingBase> MusicVolRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRSliderSettingBase> SoundFXVolRow;
	/* ~Audio */

	/* Developer */
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRToggleSettingBase> InvincibilityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRToggleSettingBase> ViewModeUnlitRow;
	/* ~Developer */

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> ConfirmResText;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> ConfirmResButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> RevertResButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> RestartGameText;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> RestartButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> ExitRestartButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> ExitButton;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> SwapScreenAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ConfirmResAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> RestartAnim;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SettingsWidget")
		TObjectPtr<UUserWidget> ParentWidget;
	
	UFUNCTION(BlueprintCallable, Category = "SettingsWidget")
		void RefreshUI();

protected:
	
	DECLARE_MULTICAST_DELEGATE(FOnRefreshDisplay);
	FOnRefreshDisplay OnRefreshDisplay;

	UPROPERTY(BlueprintReadOnly, Category = "Settings") uint8 ScreenIndex;
	UPROPERTY(Transient) TObjectPtr<class UGameSettings> SettingsObj;

	TSet<FString> RequiresRestart;
	TArray<FIntPoint> Resolutions;
	int32 LastConfirmedResIdx;
	bool bIsFinalResolution;
	float ResolutionWaitTime;
	float AutoDetectWaitTime;

	void SetScreenIndex(const uint8 InIndex);
	void OnGeneralButtonClicked() { SetScreenIndex(0); }
	void OnGraphicsButtonClicked() { SetScreenIndex(1); }
	void OnAudioButtonClicked() { SetScreenIndex(2); }
	void OnKeybindsButtonClicked() { SetScreenIndex(3); }
	void OnDebuggingButtonClicked() { SetScreenIndex(4); }

	void RefreshScalability();
	void RefreshResolutions();
	void OnUpdateResolution();
	void OnOverallQualityChanged(int32 Index, FName Value);
	void OnAnyScalabilityChanged(int32 Index, FName Value);

	UFUNCTION() void OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnAutoDetectClicked();
	UFUNCTION() void OnConfirmResClicked();
	UFUNCTION() void OnRevertResClicked();
	UFUNCTION() void OnRestartClicked();
	UFUNCTION() void OnExitClicked();

	virtual void InitWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& InGeometry, float DeltaTime) override;
};
