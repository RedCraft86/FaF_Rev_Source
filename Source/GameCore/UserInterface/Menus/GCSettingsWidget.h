// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "UserInterface/GCUserWidget.h"
#include "GCSettingsWidget.generated.h"

class UButton;
class UGCAnimatedButton;
class UGCToggleSetting;
class UGCSliderSetting;
class UGCSwitcherSetting;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSettingsBackPressedBP);

UCLASS(Abstract, DisplayName = "Settings Widget Base")
class GAMECORE_API UGCSettingsWidget : public UGCUserWidget
{
	GENERATED_BODY()

public:

	UGCSettingsWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, DisplayName = "On Exit")
		FOnSettingsBackPressedBP OnExitBP;

	UPROPERTY(Transient, meta = (BindWidget))
		UGCAnimatedButton* GeneralButton;

	UPROPERTY(Transient, meta = (BindWidget))
		UGCAnimatedButton* GraphicsButton;

	UPROPERTY(Transient, meta = (BindWidget))
		UGCAnimatedButton* AudioButton;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UGCAnimatedButton* KeybindsButton;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UGCAnimatedButton* DebuggingButton;
	
	/* General */
	UPROPERTY(Transient, meta = (BindWidget))
		UGCToggleSetting* ToggleFramerateRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSliderSetting* FieldOfViewRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UGCToggleSetting* CameraSmoothingRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSliderSetting* XSensitivityRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSliderSetting* YSensitivityRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UGCToggleSetting* XInvertRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UGCToggleSetting* YInvertRow;
	/* ~General */

	/* Graphics */
	// Video
	UPROPERTY(Transient, meta = (BindWidget))
		class UComboBoxString* ResolutionBox;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSliderSetting* ResScaleRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UGCToggleSetting* VSyncRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSwitcherSetting* FrameRateRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSliderSetting* GammaRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSliderSetting* BrightnessRow;
	
	// Color Blind
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSwitcherSetting* CBlindModeRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSliderSetting* CBlindStrengthRow;
	
	// Rendering Features
	UPROPERTY(Transient, meta = (BindWidget))
		UGCToggleSetting* FancyBloomRow;

	UPROPERTY(Transient, meta = (BindWidget))
		UGCSwitcherSetting* MBlurAmountRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSwitcherSetting* AAliasingTypeRow;

	UPROPERTY(Transient, meta = (BindWidget))
		UGCSwitcherSetting* FSRQualityRow;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UGCToggleSetting* FSRFrameInterpRow;
	
	// Advanced
	UPROPERTY(Transient, meta = (BindWidget))
		UButton* AutoDetectButton;

	UPROPERTY(Transient, meta = (BindWidget))
		class UTextBlock* AutoDetectText;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSwitcherSetting* OverallQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSwitcherSetting* ViewDistQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSwitcherSetting* AAliasingQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSwitcherSetting* ShadowQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSwitcherSetting* GIlluminationQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSwitcherSetting* ReflectionQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSwitcherSetting* PostProcessQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSwitcherSetting* TextureQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSwitcherSetting* EffectsQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSwitcherSetting* FoliageQualityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSwitcherSetting* ShadingQualityRow;
	/* ~Graphics */

	/* Audio */
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSliderSetting* MasterVolRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSliderSetting* AmbienceVolRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSliderSetting* MusicVolRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UGCSliderSetting* SoundFXVolRow;
	/* ~Audio */

	/* Developer */
	UPROPERTY(Transient, meta = (BindWidget))
		UGCToggleSetting* InvincibilityRow;
		
	UPROPERTY(Transient, meta = (BindWidget))
		UGCToggleSetting* ViewModeUnlitRow;
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
	
	UFUNCTION(BlueprintCallable, Category = "SettingsWidget")
		void RefreshDisplay();
	
	DECLARE_MULTICAST_DELEGATE(FOnBackPressed);
	FOnBackPressed OnExit;
	
protected:

	DECLARE_MULTICAST_DELEGATE(FOnRefreshDisplay);
	FOnRefreshDisplay OnRefreshDisplay;

	UPROPERTY(Transient)
		class UGCUserSettings* SettingsObject;

	UPROPERTY(BlueprintReadOnly, Category = "SettingsWidget")
		int32 ScreenIndex;
	
	TArray<FIntPoint> Resolutions;
	int32 LastConfirmedRes;
	bool bIsFinalResolution;
	float ResolutionWaitTime;
	float AutoDetectWaitTime;

	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& InGeometry, float DeltaTime) override;

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

	UFUNCTION()
		void OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
		void OnAutoDetectClicked();

	UFUNCTION()
		void OnConfirmResClicked();

	UFUNCTION()
		void OnRevertResClicked();
	
	UFUNCTION()
		void OnExitClicked();
};
