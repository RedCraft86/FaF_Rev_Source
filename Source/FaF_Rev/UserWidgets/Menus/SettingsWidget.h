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
		UFRAnimatedButtonBase* GeneralButton;

	UPROPERTY(Transient, meta = (BindWidget))
		UFRAnimatedButtonBase* GraphicsButton;

	UPROPERTY(Transient, meta = (BindWidget))
		UFRAnimatedButtonBase* AudioButton;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UFRAnimatedButtonBase* KeybindsButton;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UFRAnimatedButtonBase* DebuggingButton;
	
	/* General */
	UPROPERTY(Transient, meta = (BindWidget))
		UFRToggleSettingBase* ToggleFramerateRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSliderSettingBase* FieldOfViewRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UFRToggleSettingBase* CameraSmoothingRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSliderSettingBase* XSensitivityRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSliderSettingBase* YSensitivityRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UFRToggleSettingBase* XInvertRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UFRToggleSettingBase* YInvertRow;
	/* ~General */

	/* Graphics */
	// Video
	UPROPERTY(Transient, meta = (BindWidget))
		class UComboBoxString* ResolutionBox;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSliderSettingBase* ResScaleRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UFRToggleSettingBase* VSyncRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSwitcherSettingBase* FrameRateRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSliderSettingBase* GammaRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSliderSettingBase* BrightnessRow;
	
	// Color Blind
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSwitcherSettingBase* CBlindModeRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSliderSettingBase* CBlindStrengthRow;
	
	// Rendering Features
	UPROPERTY(Transient, meta = (BindWidget))
		UFRToggleSettingBase* FancyBloomRow;

	UPROPERTY(Transient, meta = (BindWidget))
		UFRSwitcherSettingBase* MBlurAmountRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSwitcherSettingBase* AAliasingTypeRow;

	// UPROPERTY(Transient, meta = (BindWidget))
	// 	UFRSwitcherSettingBase* FSRQualityRow;
	//
	// UPROPERTY(Transient, meta = (BindWidget))
	// 	UFRToggleSettingBase* FSRFrameInterpRow;
	
	// Advanced
	UPROPERTY(Transient, meta = (BindWidget))
		UButton* AutoDetectButton;

	UPROPERTY(Transient, meta = (BindWidget))
		class UTextBlock* AutoDetectText;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSwitcherSettingBase* OverallQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSwitcherSettingBase* ViewDistQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSwitcherSettingBase* AAliasingQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSwitcherSettingBase* ShadowQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSwitcherSettingBase* GIlluminationQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSwitcherSettingBase* ReflectionQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSwitcherSettingBase* PostProcessQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSwitcherSettingBase* TextureQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSwitcherSettingBase* EffectsQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSwitcherSettingBase* FoliageQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSwitcherSettingBase* ShadingQualityRow;
	/* ~Graphics */

	/* Audio */
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSliderSettingBase* MasterVolRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSliderSettingBase* AmbienceVolRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSliderSettingBase* MusicVolRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UFRSliderSettingBase* SoundFXVolRow;
	/* ~Audio */

	/* Developer */
	UPROPERTY(Transient, meta = (BindWidget))
		UFRToggleSettingBase* InvincibilityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UFRToggleSettingBase* ViewModeUnlitRow;
	/* ~Developer */

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* ConfirmResText;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UButton* ConfirmResButton;

	UPROPERTY(Transient, meta = (BindWidget))
		UButton* RevertResButton;

	UPROPERTY(Transient, meta = (BindWidget))
		UButton* ExitButton;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* SwapScreenAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* ConfirmResAnim;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SettingsWidget")
		UUserWidget* ParentWidget;
	
	UFUNCTION(BlueprintCallable, Category = "SettingsWidget")
		void RefreshUI();

protected:
	
	DECLARE_MULTICAST_DELEGATE(FOnRefreshDisplay);
	FOnRefreshDisplay OnRefreshDisplay;

	uint8 ScreenIndex;
	UPROPERTY(Transient) class UGameSettings* SettingsObj;

	TArray<FIntPoint> Resolutions;
	int32 LastConfirmedResIdx;
	bool bIsFinalResolution;
	float ResolutionWaitTime;
	float AutoDetectWaitTime;

	void SetScreenIndex(const int32 InIndex);
	void OnGeneralButtonClicked() { SetScreenIndex(0); }
	void OnGraphicsButtonClicked() { SetScreenIndex(1); }
	void OnAudioButtonClicked() { SetScreenIndex(2); }
	void OnKeybindsButtonClicked() { SetScreenIndex(3); }
	void OnDebuggingButtonClicked() { SetScreenIndex(4); }

	void RefreshResolutions();
	void OnUpdateResolution();
	void OnOverallQualityChanged(int32 Index, FName Value);
	void OnAnyScalabilityChanged(int32 Index, FName Value);

	UFUNCTION() void OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnAutoDetectClicked();
	UFUNCTION() void OnConfirmResClicked();
	UFUNCTION() void OnRevertResClicked();
	UFUNCTION() void OnExitClicked();

	virtual void InitWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& InGeometry, float DeltaTime) override;
};
