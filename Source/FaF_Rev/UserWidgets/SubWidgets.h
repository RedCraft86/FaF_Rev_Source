// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Components/RichTextBlockImageDecorator.h"
#include "SubWidgets.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimpleSubWidgetDelegateBP);

UCLASS(Abstract, DisplayName = "Animated Button Base")
class FAF_REV_API UFRAnimatedButtonBase final : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFRAnimatedButtonBase(const FObjectInitializer& ObjectInitializer)
		: UUserWidget(ObjectInitializer), Button(nullptr), ButtonAnim(nullptr)
	{
#if WITH_EDITORONLY_DATA
		PaletteCategory = NSLOCTEXT("UMG", "Common", "Common");
#endif
	}

	UPROPERTY(BlueprintAssignable, DisplayName = "On Clicked")
		FSimpleSubWidgetDelegateBP OnClickedBP;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> Button;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ButtonAnim;

	DECLARE_MULTICAST_DELEGATE(FOnClicked);
	FOnClicked OnClicked;

	virtual void NativeOnInitialized() override;
	
protected:

	UFUNCTION() void OnButtonClicked();
	UFUNCTION() void OnButtonHovered();
	UFUNCTION() void OnButtonUnhovered();
};

UCLASS(Abstract, DisplayName = "Keybind Row Base")
class FAF_REV_API UFRKeybindRowBase final : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFRKeybindRowBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> LabelText;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UHorizontalBox> KeybindsBox;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (DisplayThumbnail = false))
		TObjectPtr<class UInputMappingContext> MappingContext;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (DisplayThumbnail = false))
		TObjectPtr<class UInputAction> Action;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		FVector2D DesiredSize;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FSlateBrush DividerBrush;

	UFUNCTION(BlueprintImplementableEvent)
		FRichImageRow GetIconForKey(const FName& InKey);
	
protected:
	
	virtual void NativePreConstruct() override;
};

UCLASS(Abstract, NotBlueprintable, DisplayName = "Setting Row Base")
class FAF_REV_API UFRSettingRowBase : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFRSettingRowBase(const FObjectInitializer& ObjectInitializer)
		: UUserWidget(ObjectInitializer), LabelText(nullptr), ResetButton(nullptr), Label(INVTEXT("Setting"))
	{
#if WITH_EDITORONLY_DATA
		PaletteCategory = NSLOCTEXT("FaF_Rev", "UMGSettingRows", "Setting Rows");
#endif
	}

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> LabelText;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> ResetButton;

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
	
	UFUNCTION() void OnResetClicked();
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToggleValueChanged, bool, bEnabled);

UCLASS(Abstract, Blueprintable, DisplayName = "Toggle Setting Row Base")
class FAF_REV_API UFRToggleSettingBase final : public UFRSettingRowBase
{
	GENERATED_BODY()
	
public:

	UFRToggleSettingBase(const FObjectInitializer& ObjectInitializer)
		: UFRSettingRowBase(ObjectInitializer), ToggleButton(nullptr), ToggleLabelText(nullptr)
		, bDefaultValue(false), EnabledLabel(TEXT("Enabled")), EnabledColor(FLinearColor::Yellow)
		, DisabledLabel(TEXT("Disabled")), DisabledColor(FLinearColor::White), bCurrentValue(false)
		, GetterFunction(nullptr), SetterFunction(nullptr)
	{}

	UPROPERTY(BlueprintAssignable, DisplayName = "On Value Changed")
		FOnToggleValueChanged OnValueChangedBP;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> ToggleButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> ToggleLabelText;

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
	
	UFUNCTION() void OnToggleButtonClicked();
	
	virtual void NativeOnInitialized() override;
#if WITH_EDITOR
	virtual void NativePreConstruct() override;
#endif
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSliderValueChanged, float, Value);

UCLASS(Abstract, Blueprintable, DisplayName = "Slider Setting Row Base")
class FAF_REV_API UFRSliderSettingBase final : public UFRSettingRowBase
{
	GENERATED_BODY()
	
public:

	UFRSliderSettingBase(const FObjectInitializer& ObjectInitializer)
		: UFRSettingRowBase(ObjectInitializer), SliderSpinBox(nullptr), DefaultValue(0.0f), MinValue(0.0f), MaxValue(100.0f)
		, NumOfDecimals(0), bBroadcastAnyChange(false), bHaveDoneInit(false), GetterFunction(nullptr), SetterFunction(nullptr)
	{}

	UPROPERTY(BlueprintAssignable, DisplayName = "On Value Changed")
		FOnSliderValueChanged OnValueChangedBP;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class USpinBox> SliderSpinBox;

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

	UFUNCTION() void OnSliderValueChanged(float Value);
	UFUNCTION() void OnSliderMovementEnd(float Value);
	UFUNCTION() void OnSliderValueCommitted(float Value, ETextCommit::Type Type);

	void BroadcastChange() const;
	void ApplySliderSettings() const;
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
#if WITH_EDITOR
	virtual void NativePreConstruct() override;
#endif
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSwitcherValueChanged, int32, Index, FName, Value);

UCLASS(Abstract, Blueprintable, DisplayName = "Switcher Setting Row Base")
class FAF_REV_API UFRSwitcherSettingBase final : public UFRSettingRowBase
{
	GENERATED_BODY()
	
public:

	UFRSwitcherSettingBase(const FObjectInitializer& ObjectInitializer)
		: UFRSettingRowBase(ObjectInitializer), ValueText(nullptr), PrevButton(nullptr), NextButton(nullptr)
		, DefaultValue(0), ValueOptions({}), OptionTooltips({}), OptionTooltip(FText::GetEmpty())
		, CurrentIdx(0), GetterFunction(nullptr), SetterFunction(nullptr)
	{}

	UPROPERTY(BlueprintAssignable, DisplayName = "On Value Changed")
		FOnSwitcherValueChanged OnValueChangedBP;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> ValueText;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> PrevButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> NextButton;

	UPROPERTY(EditAnywhere, Category = "Settings")
		uint8 DefaultValue;
	
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
	void AssignGetterFunction(const TFunction<uint8()>& Function) { if (Function) GetterFunction = Function; }
	void AssignSetterFunction(const TFunction<void(const uint8)>& Function) { if (Function) SetterFunction = Function; }
	
protected:
	
	uint8 CurrentIdx;
	TFunction<uint8()> GetterFunction;
	TFunction<void(const uint8)> SetterFunction;

	UFUNCTION() void OnPrevClicked();
	UFUNCTION() void OnNextClicked();
	
	virtual void NativeOnInitialized() override;
#if WITH_EDITOR
	virtual void NativePreConstruct() override;
#endif
};