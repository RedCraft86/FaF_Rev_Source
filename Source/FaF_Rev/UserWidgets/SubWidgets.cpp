// Copyright (C) RedCraft86. All Rights Reserved.

#include "SubWidgets.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/SpinBox.h"
#include "Components/TextBlock.h"

void UFRAnimatedButtonBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Button->OnClicked.AddDynamic(this, &UFRAnimatedButtonBase::OnButtonClicked);
	Button->OnHovered.AddDynamic(this, &UFRAnimatedButtonBase::OnButtonHovered);
	Button->OnUnhovered.AddDynamic(this, &UFRAnimatedButtonBase::OnButtonUnhovered);
}

void UFRAnimatedButtonBase::OnButtonClicked()
{
	OnClicked.Broadcast();
	OnClickedBP.Broadcast();
}

void UFRAnimatedButtonBase::OnButtonHovered()
{
	PlayAnimationForward(ButtonAnim);
}

void UFRAnimatedButtonBase::OnButtonUnhovered()
{
	PlayAnimationReverse(ButtonAnim);
}

UFRKeybindRowBase::UFRKeybindRowBase(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer), LabelText(nullptr), KeybindsBox(nullptr)
{
	Label = FText::FromString("Keybind");
	Keys = {};

	DividerBrush.TintColor = FLinearColor::Gray;
	DividerBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
	DividerBrush.ImageSize = FVector2D(4.0f, 32.0f);
	DividerBrush.OutlineSettings.CornerRadii = FVector4(2.0f, 2.0f, 2.0f, 2.0f);
	DividerBrush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
	DividerBrush.OutlineSettings.bUseBrushTransparency = true;
}

void UFRKeybindRowBase::CreateIcons()
{
	KeybindsBox->ClearChildren();
	for (int32 i = 0; i < Keys.Num(); i++)
	{
		UImage* IconImage = WidgetTree->ConstructWidget<UImage>();
		IconImage->SetBrush(GetIconForKey(Keys[i]));

		UHorizontalBoxSlot* IconSlot = Cast<UHorizontalBoxSlot>(KeybindsBox->AddChild(IconImage));
		IconSlot->SetHorizontalAlignment(HAlign_Center);
		IconSlot->SetVerticalAlignment(VAlign_Center);

		if (Keys.Num() != 1 && i != Keys.Num() - 1)
		{
			UImage* DividerImage = WidgetTree->ConstructWidget<UImage>();
			DividerImage->SetBrush(DividerBrush);
						
			UHorizontalBoxSlot* DividerSlot = Cast<UHorizontalBoxSlot>(KeybindsBox->AddChild(DividerImage));
			DividerSlot->SetPadding(FMargin(5.0f, 0.0f));
			DividerSlot->SetHorizontalAlignment(HAlign_Center);
			DividerSlot->SetVerticalAlignment(VAlign_Fill);
		}
	}
}

void UFRKeybindRowBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (LabelText) LabelText->SetText(Label);
	CreateIcons();
}

void UFRSettingRowBase::CheckDefaultValue()
{
	ResetButton->SetVisibility(IsDefaultValue() ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
}

void UFRSettingRowBase::NativeConstruct()
{
	Super::NativeConstruct();
	CheckDefaultValue();
}

void UFRSettingRowBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ResetButton->OnClicked.AddDynamic(this, &UFRSettingRowBase::OnResetClicked);
}

void UFRSettingRowBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (LabelText) LabelText->SetText(Label);
}

void UFRSettingRowBase::OnResetClicked()
{
	if (!IsDefaultValue()) ResetValue();
}

void UFRToggleSettingBase::SetValueSilently(const bool bInValue)
{
	bCurrentValue = bInValue;
	ToggleLabelText->SetText(FText::FromName(bCurrentValue ? EnabledLabel : DisabledLabel));
	ToggleLabelText->SetColorAndOpacity(bCurrentValue ? EnabledColor : DisabledColor);
	CheckDefaultValue();
}

void UFRToggleSettingBase::SetValue(const bool bInValue)
{
	if (bCurrentValue != bInValue)
	{
		SetValueSilently(bInValue);
		if (SetterFunction) SetterFunction(bCurrentValue);
		
		OnValueChanged.Broadcast(bCurrentValue);
		OnValueChangedBP.Broadcast(bCurrentValue);
	}
}

void UFRToggleSettingBase::ResetValue()
{
	SetValue(bDefaultValue);
}

void UFRToggleSettingBase::RefreshValue()
{
	if (GetterFunction) SetValueSilently(GetterFunction());
}

bool UFRToggleSettingBase::IsDefaultValue()
{
	return bCurrentValue == bDefaultValue;
}

void UFRToggleSettingBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	bCurrentValue = bDefaultValue;
	ToggleLabelText->SetText(FText::FromName(bCurrentValue ? EnabledLabel : DisabledLabel));
	ToggleLabelText->SetColorAndOpacity(bCurrentValue ? EnabledColor : DisabledColor);
	ToggleButton->OnClicked.AddDynamic(this, &UFRToggleSettingBase::OnToggleButtonClicked);
}

#if WITH_EDITOR
void UFRToggleSettingBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (!FApp::IsGame())
	{
		bCurrentValue = bDefaultValue;
		if (ToggleLabelText)
		{
			ToggleLabelText->SetText(FText::FromName(bCurrentValue ? EnabledLabel : DisabledLabel));
			ToggleLabelText->SetColorAndOpacity(bCurrentValue ? EnabledColor : DisabledColor);
		}
	}
}
#endif

void UFRToggleSettingBase::OnToggleButtonClicked()
{
	SetValue(!bCurrentValue);
}

void UFRSliderSettingBase::SetValueSilently(const float InValue)
{
	bHaveDoneInit = false;
	SliderSpinBox->SetValue(FMath::Clamp(InValue, MinValue, MaxValue));
	CheckDefaultValue();
	bHaveDoneInit = true;
}

void UFRSliderSettingBase::SetValue(const float InValue)
{
	const float ClampedValue = FMath::Clamp(InValue, MinValue, MaxValue);
	if (SliderSpinBox->GetValue() != ClampedValue)
	{
		SliderSpinBox->SetValue(ClampedValue);
		CheckDefaultValue();

		if (!bBroadcastAnyChange)
		{
			BroadcastChange();
		}
	}
}

float UFRSliderSettingBase::GetValue() const
{
	return SliderSpinBox->GetValue();
}

void UFRSliderSettingBase::ResetValue()
{
	SetValue(DefaultValue);
}

void UFRSliderSettingBase::RefreshValue()
{
	if (GetterFunction) SetValueSilently(GetterFunction());
}

bool UFRSliderSettingBase::IsDefaultValue()
{
	return SliderSpinBox->GetValue() == DefaultValue;
}

void UFRSliderSettingBase::ApplySliderSettings() const
{
	SliderSpinBox->SetMinValue(MinValue);
	SliderSpinBox->SetMaxValue(MaxValue);
	SliderSpinBox->SetMinSliderValue(MinValue);
	SliderSpinBox->SetMaxSliderValue(MaxValue);
	SliderSpinBox->SetMaxFractionalDigits(NumOfDecimals);
	SliderSpinBox->SetMinFractionalDigits(NumOfDecimals > 0 ? 1 : 0);
	SliderSpinBox->SetDelta(NumOfDecimals > 0 ? 0.0f : 1.0f);
}

void UFRSliderSettingBase::NativeConstruct()
{
	Super::NativeConstruct();
	bHaveDoneInit = true;
}

void UFRSliderSettingBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ApplySliderSettings();
	SliderSpinBox->SetValue(FMath::Clamp(DefaultValue, MinValue, MaxValue));
	SliderSpinBox->OnValueChanged.AddDynamic(this, &UFRSliderSettingBase::OnSliderValueChanged);
	SliderSpinBox->OnEndSliderMovement.AddDynamic(this, &UFRSliderSettingBase::OnSliderMovementEnd);
	SliderSpinBox->OnValueCommitted.AddDynamic(this, &UFRSliderSettingBase::OnSliderValueCommitted);
}

#if WITH_EDITOR
void UFRSliderSettingBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (!FApp::IsGame())
	{
		DefaultValue = FMath::Clamp(DefaultValue, MinValue, MaxValue);
		if (SliderSpinBox)
		{
			ApplySliderSettings();
			SliderSpinBox->SetValue(DefaultValue);
		}
	}
}
#endif

void UFRSliderSettingBase::BroadcastChange() const
{
	const float Val = SliderSpinBox->GetValue();
	if (SetterFunction) SetterFunction(Val);
	
	OnValueChanged.Broadcast(Val);
	OnValueChangedBP.Broadcast(Val);
}

void UFRSliderSettingBase::OnSliderValueChanged(float Value)
{
	if (bHaveDoneInit && bBroadcastAnyChange)
	{
		CheckDefaultValue();
		BroadcastChange();
	}
}

void UFRSliderSettingBase::OnSliderMovementEnd(float Value)
{
	if (!bBroadcastAnyChange)
	{
		CheckDefaultValue();
		BroadcastChange();
	}
}

void UFRSliderSettingBase::OnSliderValueCommitted(float Value, ETextCommit::Type Type)
{
	if (!bBroadcastAnyChange)
	{
		CheckDefaultValue();
		BroadcastChange();
	}
}

void UFRSwitcherSettingBase::SetValueSilently(const int32 InValue)
{
	if (InValue >= 0 && ValueOptions.IsValidIndex(InValue))
	{
		CurrentIdx = InValue;
		CheckDefaultValue();

		const FName AsName = ValueOptions[CurrentIdx];
		ValueText->SetText(FText::FromName(AsName));
		OptionTooltip = OptionTooltips.FindRef(AsName);
	}
}

void UFRSwitcherSettingBase::SetValue(const int32 InValue)
{
	if (CurrentIdx != InValue && InValue >= 0 && ValueOptions.IsValidIndex(InValue))
	{
		SetValueSilently(InValue);

		if (SetterFunction) SetterFunction(CurrentIdx);
		
		const FName AsName = ValueOptions[CurrentIdx];
		OnValueChanged.Broadcast(CurrentIdx, AsName);
		OnValueChangedBP.Broadcast(CurrentIdx, AsName);
	}
}

void UFRSwitcherSettingBase::ResetValue()
{
	SetValue(DefaultValue);
}

void UFRSwitcherSettingBase::RefreshValue()
{
	if (GetterFunction) SetValueSilently(GetterFunction());
}

bool UFRSwitcherSettingBase::IsDefaultValue()
{
	return CurrentIdx == DefaultValue;
}

void UFRSwitcherSettingBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ValueOptions.Remove(NAME_None);
	CurrentIdx = FMath::Clamp(DefaultValue, 0, ValueOptions.Num() - 1);
	OptionTooltip = OptionTooltips.FindRef(ValueOptions[CurrentIdx]);
	ValueText->SetText(FText::FromName(ValueOptions[CurrentIdx]));
	
	PrevButton->OnClicked.AddDynamic(this, &UFRSwitcherSettingBase::OnPrevClicked);
	NextButton->OnClicked.AddDynamic(this, &UFRSwitcherSettingBase::OnNextClicked);
}

#if WITH_EDITOR
void UFRSwitcherSettingBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (!FApp::IsGame())
	{
		FName TargetText = NAME_None;
		if (ValueOptions.IsEmpty())
		{
			ValueOptions.Add(TEXT("Option"));
			DefaultValue = 0;
			CurrentIdx = 0;
		}
		else
		{
			DefaultValue = FMath::Clamp(DefaultValue, 0, ValueOptions.Num() - 1);
			CurrentIdx = DefaultValue;
			
			TargetText = ValueOptions[CurrentIdx];
		}

		if (ValueText)
		{
			ValueText->SetText(FText::FromName(TargetText));
		}
	}
}
#endif

void UFRSwitcherSettingBase::OnPrevClicked()
{
	int32 TargetIdx = CurrentIdx - 1;
	if (TargetIdx < 0) TargetIdx = ValueOptions.Num() - 1;
	SetValue(TargetIdx);
}

void UFRSwitcherSettingBase::OnNextClicked()
{
	int32 TargetIdx = CurrentIdx + 1;
	if (TargetIdx >= ValueOptions.Num()) TargetIdx = 0;
	SetValue(TargetIdx);
}
