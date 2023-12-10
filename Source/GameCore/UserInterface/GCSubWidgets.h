// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "GCSubWidgets.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimpleSubWidgetDelegateBP);

UCLASS(Abstract, DisplayName = "Animated Button Base")
class GAMECORE_API UGCAnimatedButton : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UGCAnimatedButton(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, DisplayName = "On Clicked")
		FSimpleSubWidgetDelegateBP OnClickedBP;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UButton* Button;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* ButtonAnim;

	DECLARE_MULTICAST_DELEGATE(FOnClicked);
	FOnClicked OnClicked;

	virtual void NativeOnInitialized() override;
	
protected:

	UFUNCTION()
		virtual void OnButtonClicked();

	UFUNCTION()
		virtual void OnButtonHovered();

	UFUNCTION()
		virtual void OnButtonUnhovered();
};

UCLASS(Abstract, DisplayName = "Keybind Row Base")
class GAMECORE_API UGCKeybindRowBase : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UGCKeybindRowBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* LabelText;
	
	UPROPERTY(Transient, meta = (BindWidget))
		class UHorizontalBox* KeybindsBox;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FText Label;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (TitleProperty = "ResourceObject"))
		TArray<FSlateBrush> Icons;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FSlateBrush DividerBrush;
	
protected:

	void CreateIcons();
	virtual void NativePreConstruct() override;
};

UCLASS(Abstract, NotBlueprintable, DisplayName = "Setting Row Base")
class GAMECORE_API UGCSettingRowBase : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UGCSettingRowBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* LabelText;

	UPROPERTY(Transient, meta = (BindWidget))
		UButton* ResetButton;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (DisplayPriority = -1))
		FText Label;

	virtual void ResetValue() {}
	virtual void RefreshValue() {}
	virtual bool IsDefaultValue() { return true; }
	
protected:

	void CheckDefaultValue();
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;

private:
	
	UFUNCTION()
		void OnResetClicked();
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToggleValueChanged, bool, bEnabled);

UCLASS(Abstract, Blueprintable, DisplayName = "Toggle Setting Row Base")
class GAMECORE_API UGCToggleSetting : public UGCSettingRowBase
{
	GENERATED_BODY()
	
public:

	UGCToggleSetting(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, DisplayName = "On Value Changed")
		FOnToggleValueChanged OnValueChangedBP;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UButton* ToggleButton;

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* ToggleLabelText;

	UPROPERTY(EditAnywhere, Category = "Settings")
		bool bDefaultValue;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		FName EnabledLabel;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FLinearColor EnabledColor;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FName DisabledLabel;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FLinearColor DisabledColor;

	UFUNCTION(BlueprintCallable, Category = "SettingRow|Toggle")
		void SetValueSilently(const bool bInValue);
	
	UFUNCTION(BlueprintCallable, Category = "SettingRow|Toggle")
		void SetValue(const bool bInValue);
	
	UFUNCTION(BlueprintPure, Category = "SettingRow|Toggle")
		bool GetValue() const { return bCurrentValue; }
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnValueChanged, bool);
	FOnValueChanged OnValueChanged;
	
	virtual void ResetValue() override;
	virtual void RefreshValue() override;
	virtual bool IsDefaultValue() override;
	void AssignGetterFunction(const TFunction<bool()>& Function) { if (Function) GetterFunction = Function; }
	void AssignSetterFunction(const TFunction<void(const bool)>& Function) { if (Function) SetterFunction = Function; }
	
protected:
	
	bool bCurrentValue;
	TFunction<bool()> GetterFunction;
	TFunction<void(const bool)> SetterFunction;
	
	virtual void NativeOnInitialized() override;
#if WITH_EDITOR
	virtual void NativePreConstruct() override;
#endif

	UFUNCTION()
		void OnToggleButtonClicked();
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSliderValueChanged, float, Value);

UCLASS(Abstract, Blueprintable, DisplayName = "Slider Setting Row Base")
class GAMECORE_API UGCSliderSetting : public UGCSettingRowBase
{
	GENERATED_BODY()
	
public:

	UGCSliderSetting(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, DisplayName = "On Value Changed")
		FOnSliderValueChanged OnValueChangedBP;

	UPROPERTY(Transient, meta = (BindWidget))
		class USpinBox* SliderSpinBox;

	UPROPERTY(EditAnywhere, Category = "Settings")
		float DefaultValue;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		float MinValue;

	UPROPERTY(EditAnywhere, Category = "Settings")
		float MaxValue;

	UPROPERTY(EditAnywhere, Category = "Settings")
		uint8 NumOfDecimals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		bool bBroadcastAnyChange;

	UFUNCTION(BlueprintCallable, Category = "SettingRow|Slider")
		void SetValueSilently(const float InValue);
	
	UFUNCTION(BlueprintCallable, Category = "SettingRow|Slider")
		void SetValue(const float InValue);
	
	UFUNCTION(BlueprintPure, Category = "SettingRow|Slider")
		float GetValue() const;
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnValueChanged, float);
	FOnValueChanged OnValueChanged;
	
	virtual void ResetValue() override;
	virtual void RefreshValue() override;
	virtual bool IsDefaultValue() override;
	void AssignGetterFunction(const TFunction<float()>& Function) { if (Function) GetterFunction = Function; }
	void AssignSetterFunction(const TFunction<void(const float)>& Function) { if (Function) SetterFunction = Function; }
	
protected:
	
	bool bHaveDoneInit;
	TFunction<float()> GetterFunction;
	TFunction<void(const float)> SetterFunction;
	
	void ApplySliderSettings() const;
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
#if WITH_EDITOR
	virtual void NativePreConstruct() override;
#endif

	void BroadcastChange() const;
	
	UFUNCTION()
		void OnSliderValueChanged(float Value);

	UFUNCTION()
		void OnSliderMovementEnd(float Value);
	
	UFUNCTION()
		void OnSliderValueCommitted(float Value, ETextCommit::Type Type);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSwitcherValueChanged, int32, Index, FName, Value);

UCLASS(Abstract, Blueprintable, DisplayName = "Switcher Setting Row Base")
class GAMECORE_API UGCSwitcherSetting : public UGCSettingRowBase
{
	GENERATED_BODY()
	
public:

	UGCSwitcherSetting(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, DisplayName = "On Value Changed")
		FOnSwitcherValueChanged OnValueChangedBP;

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* ValueText;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UButton* PrevButton;

	UPROPERTY(Transient, meta = (BindWidget))
		UButton* NextButton;

	UPROPERTY(EditAnywhere, Category = "Settings")
		int32 DefaultValue;
	
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (NoElementDuplicate))
		TArray<FName> ValueOptions;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (GetOptions = "ValueOptions", MultiLine = true))
		TMap<FName, FText> OptionTooltips;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
		FText OptionTooltip;
	
	UFUNCTION(BlueprintCallable, Category = "SettingRow|Switcher")
		void SetValueSilently(const int32 InValue);
	
	UFUNCTION(BlueprintCallable, Category = "SettingRow|Switcher")
		void SetValue(const int32 InValue);
	
	UFUNCTION(BlueprintPure, Category = "SettingRow|Switcher")
		FName GetSelectedValue() const { return CurrentIdx != INDEX_NONE ? ValueOptions[CurrentIdx] : NAME_None; }

	UFUNCTION(BlueprintPure, Category = "SettingRow|Switcher")
		int32 GetSelectedIndex() const { return CurrentIdx; }
	
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnValueChanged, int32, FName);
	FOnValueChanged OnValueChanged;
	
	virtual void ResetValue() override;
	virtual void RefreshValue() override;
	virtual bool IsDefaultValue() override;
	void AssignGetterFunction(const TFunction<int32()>& Function) { if (Function) GetterFunction = Function; }
	void AssignSetterFunction(const TFunction<void(const int32)>& Function) { if (Function) SetterFunction = Function; }
	
protected:
	
	int32 CurrentIdx;
	TFunction<int32()> GetterFunction;
	TFunction<void(const int32)> SetterFunction;
	
	virtual void NativeOnInitialized() override;
#if WITH_EDITOR
	virtual void NativePreConstruct() override;
#endif

	UFUNCTION()
		void OnPrevClicked();

	UFUNCTION()
		void OnNextClicked();
};