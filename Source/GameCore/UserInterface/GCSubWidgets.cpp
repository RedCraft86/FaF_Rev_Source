// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCSubWidgets.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/SpinBox.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "InputMappingContext.h"
#include "Core/GCSettings.h"

UGCAnimatedButton::UGCAnimatedButton(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	Button = nullptr;
	ButtonAnim = nullptr;
}

void UGCAnimatedButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button->OnClicked.AddDynamic(this, &UGCAnimatedButton::OnButtonClicked);
	Button->OnHovered.AddDynamic(this, &UGCAnimatedButton::OnButtonHovered);
	Button->OnUnhovered.AddDynamic(this, &UGCAnimatedButton::OnButtonUnhovered);
}

void UGCAnimatedButton::OnButtonClicked()
{
	OnClicked.Broadcast();
	OnClickedBP.Broadcast();
}

void UGCAnimatedButton::OnButtonHovered()
{
	PlayAnimationForward(ButtonAnim);
}

void UGCAnimatedButton::OnButtonUnhovered()
{
	PlayAnimationReverse(ButtonAnim);
}

UGCKeybindRowBase::UGCKeybindRowBase(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	LabelText = nullptr;
	KeybindsBox = nullptr;
	
	Label = FText::FromString("Keybind");
	Icons = {};

	DividerBrush.TintColor = FLinearColor::Gray;
	DividerBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
	DividerBrush.ImageSize = FVector2D(4.0f, 32.0f);
	DividerBrush.OutlineSettings.CornerRadii = FVector4(2.0f, 2.0f, 2.0f, 2.0f);
	DividerBrush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
	DividerBrush.OutlineSettings.bUseBrushTransparency = true;
}

void UGCKeybindRowBase::CreateIcons()
{
	if (KeybindsBox)
	{
		KeybindsBox->ClearChildren();
		for (int32 i = 0; i < Icons.Num(); i++)
		{
			EObjectFlags NewObjectFlags = RF_Transactional;
			if (HasAnyFlags(RF_Transient))
			{
				NewObjectFlags |= RF_Transient;
			}
				
			UImage* IconImage = NewObject<UImage>(this, NAME_None, NewObjectFlags);
			IconImage->SetBrush(Icons[i]);
					
			UHorizontalBoxSlot* IconSlot = Cast<UHorizontalBoxSlot>(KeybindsBox->AddChild(IconImage));
			IconSlot->SetHorizontalAlignment(HAlign_Center);
			IconSlot->SetVerticalAlignment(VAlign_Center);

			if (Icons.Num() != 1 && i != Icons.Num() - 1)
			{
				UImage* DividerImage = NewObject<UImage>(this, NAME_None, NewObjectFlags);
				DividerImage->SetBrush(DividerBrush);
						
				UHorizontalBoxSlot* DividerSlot = Cast<UHorizontalBoxSlot>(KeybindsBox->AddChild(DividerImage));
				DividerSlot->SetPadding(FMargin(5.0f, 0.0f));
				DividerSlot->SetHorizontalAlignment(HAlign_Center);
				DividerSlot->SetVerticalAlignment(VAlign_Fill);
			}
		}
	}
}

void UGCKeybindRowBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (LabelText)
	{
		LabelText->SetText(Label);
	}

	CreateIcons();
}

UGCSettingRowBase::UGCSettingRowBase(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	LabelText = nullptr;
	ResetButton = nullptr;
	
	Label = FText::FromString("Setting Row");
}

void UGCSettingRowBase::CheckDefaultValue()
{
	ResetButton->SetVisibility(IsDefaultValue() ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
}

void UGCSettingRowBase::NativeConstruct()
{
	Super::NativeConstruct();
	CheckDefaultValue();
}

void UGCSettingRowBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ResetButton->OnClicked.AddDynamic(this, &UGCSettingRowBase::OnResetClicked);
}

void UGCSettingRowBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (LabelText)
	{
		LabelText->SetText(Label);
	}
}

void UGCSettingRowBase::OnResetClicked()
{
	if (!IsDefaultValue())
	{
		ResetValue();
	}
}

UGCToggleSetting::UGCToggleSetting(const FObjectInitializer& ObjectInitializer)
	: UGCSettingRowBase(ObjectInitializer)
{
	ToggleButton = nullptr;
	ToggleLabelText = nullptr;
	
	bDefaultValue = false;
	EnabledLabel = TEXT("Enabled");
	EnabledColor = FLinearColor::Yellow;
	DisabledLabel = TEXT("Disabled");
	DisabledColor = FLinearColor::White;
	bCurrentValue = false;

	GetterFunction = nullptr;
	SetterFunction = nullptr;
}

void UGCToggleSetting::SetValueSilently(const bool bInValue)
{
	bCurrentValue = bInValue;
	ToggleLabelText->SetText(FText::FromName(bCurrentValue ? EnabledLabel : DisabledLabel));
	ToggleLabelText->SetColorAndOpacity(bCurrentValue ? EnabledColor : DisabledColor);
	CheckDefaultValue();
}

void UGCToggleSetting::SetValue(const bool bInValue)
{
	if (bCurrentValue != bInValue)
	{
		SetValueSilently(bInValue);

		if (SetterFunction)
			SetterFunction(bCurrentValue);
		
		OnValueChanged.Broadcast(bCurrentValue);
		OnValueChangedBP.Broadcast(bCurrentValue);
	}
}

void UGCToggleSetting::ResetValue()
{
	Super::ResetValue();
	SetValue(bDefaultValue);
}

void UGCToggleSetting::RefreshValue()
{
	if (GetterFunction)
	{
		SetValueSilently(GetterFunction());
	}
}

bool UGCToggleSetting::IsDefaultValue()
{
	return bCurrentValue == bDefaultValue;
}

void UGCToggleSetting::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	bCurrentValue = bDefaultValue;
	ToggleLabelText->SetText(FText::FromName(bCurrentValue ? EnabledLabel : DisabledLabel));
	ToggleLabelText->SetColorAndOpacity(bCurrentValue ? EnabledColor : DisabledColor);

	ToggleButton->OnClicked.AddDynamic(this, &UGCToggleSetting::OnToggleButtonClicked);
}

#if WITH_EDITOR
void UGCToggleSetting::NativePreConstruct()
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

void UGCToggleSetting::OnToggleButtonClicked()
{
	SetValue(!bCurrentValue);
}

UGCSliderSetting::UGCSliderSetting(const FObjectInitializer& ObjectInitializer)
	: UGCSettingRowBase(ObjectInitializer)
{
	SliderSpinBox = nullptr;

	DefaultValue = 0.0f;
	MinValue = 0.0f;
	MaxValue = 100.0f;
	NumOfDecimals = 0;
	bBroadcastAnyChange = false;
	bHaveDoneInit = false;

	GetterFunction = nullptr;
	SetterFunction = nullptr;
}

void UGCSliderSetting::SetValueSilently(const float InValue)
{
	bHaveDoneInit = false;
	SliderSpinBox->SetValue(FMath::Clamp(InValue, MinValue, MaxValue));
	CheckDefaultValue();
	bHaveDoneInit = true;
}

void UGCSliderSetting::SetValue(const float InValue)
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

float UGCSliderSetting::GetValue() const
{
	return SliderSpinBox->GetValue();
}

void UGCSliderSetting::ResetValue()
{
	Super::ResetValue();
	SetValue(DefaultValue);
}

void UGCSliderSetting::RefreshValue()
{
	if (GetterFunction)
	{
		SetValueSilently(GetterFunction());
	}
}

bool UGCSliderSetting::IsDefaultValue()
{
	return SliderSpinBox->GetValue() == DefaultValue;
}

void UGCSliderSetting::ApplySliderSettings() const
{
	SliderSpinBox->SetMinValue(MinValue);
	SliderSpinBox->SetMaxValue(MaxValue);
	SliderSpinBox->SetMinSliderValue(MinValue);
	SliderSpinBox->SetMaxSliderValue(MaxValue);
	SliderSpinBox->SetMaxFractionalDigits(NumOfDecimals);
	SliderSpinBox->SetMinFractionalDigits(NumOfDecimals > 0 ? 1 : 0);
	SliderSpinBox->SetDelta(NumOfDecimals > 0 ? 0.0f : 1.0f);
}

void UGCSliderSetting::NativeConstruct()
{
	Super::NativeConstruct();
	bHaveDoneInit = true;
}

void UGCSliderSetting::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ApplySliderSettings();
	SliderSpinBox->SetValue(FMath::Clamp(DefaultValue, MinValue, MaxValue));
	SliderSpinBox->OnValueChanged.AddDynamic(this, &UGCSliderSetting::OnSliderValueChanged);
	SliderSpinBox->OnEndSliderMovement.AddDynamic(this, &UGCSliderSetting::OnSliderMovementEnd);
	SliderSpinBox->OnValueCommitted.AddDynamic(this, &UGCSliderSetting::OnSliderValueCommitted);
}

#if WITH_EDITOR
void UGCSliderSetting::NativePreConstruct()
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

void UGCSliderSetting::BroadcastChange() const
{
	const float Val = SliderSpinBox->GetValue();
	if (SetterFunction)
	{
		SetterFunction(Val);
	}
		
	OnValueChanged.Broadcast(Val);
	OnValueChangedBP.Broadcast(Val);
}

void UGCSliderSetting::OnSliderValueChanged(float Value)
{
	if (bHaveDoneInit && bBroadcastAnyChange)
	{
		CheckDefaultValue();
		BroadcastChange();
	}
}

void UGCSliderSetting::OnSliderMovementEnd(float Value)
{
	if (!bBroadcastAnyChange)
	{
		CheckDefaultValue();
		BroadcastChange();
	}
}

void UGCSliderSetting::OnSliderValueCommitted(float Value, ETextCommit::Type Type)
{
	if (!bBroadcastAnyChange)
	{
		CheckDefaultValue();
		BroadcastChange();
	}
}

UGCSwitcherSetting::UGCSwitcherSetting(const FObjectInitializer& ObjectInitializer)
	: UGCSettingRowBase(ObjectInitializer)
{
	ValueText = nullptr;
	PrevButton = nullptr;
	NextButton = nullptr;
	
	DefaultValue = -1;
	ValueOptions = {};
	OptionTooltips = {};
	OptionTooltip = FText::GetEmpty();
	CurrentIdx = INDEX_NONE;

	GetterFunction = nullptr;
	SetterFunction = nullptr;
}

void UGCSwitcherSetting::SetValueSilently(const int32 InValue)
{
	if (ValueOptions.IsValidIndex(InValue))
	{
		CurrentIdx = InValue;
		CheckDefaultValue();

		const FName AsName = ValueOptions[CurrentIdx];
		ValueText->SetText(FText::FromName(AsName));
		OptionTooltip = OptionTooltips.FindRef(AsName);
	}
}

void UGCSwitcherSetting::SetValue(const int32 InValue)
{
	if (CurrentIdx != InValue && ValueOptions.IsValidIndex(InValue))
	{
		SetValueSilently(InValue);

		if (SetterFunction)
			SetterFunction(CurrentIdx);
		
		const FName AsName = ValueOptions[CurrentIdx];
		OnValueChanged.Broadcast(CurrentIdx, AsName);
		OnValueChangedBP.Broadcast(CurrentIdx, AsName);
	}
}

void UGCSwitcherSetting::ResetValue()
{
	Super::ResetValue();
	SetValue(DefaultValue);
}

void UGCSwitcherSetting::RefreshValue()
{
	if (GetterFunction)
	{
		SetValueSilently(GetterFunction());
	}
}

bool UGCSwitcherSetting::IsDefaultValue()
{
	return CurrentIdx == DefaultValue;
}

void UGCSwitcherSetting::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ValueOptions.Remove(NAME_None);
	CurrentIdx = FMath::Clamp(DefaultValue, 0, ValueOptions.Num() - 1);
	OptionTooltip = OptionTooltips.FindRef(ValueOptions[CurrentIdx]);
	ValueText->SetText(FText::FromName(ValueOptions[CurrentIdx]));
	
	PrevButton->OnClicked.AddDynamic(this, &UGCSwitcherSetting::OnPrevClicked);
	NextButton->OnClicked.AddDynamic(this, &UGCSwitcherSetting::OnNextClicked);
}

#if WITH_EDITOR
void UGCSwitcherSetting::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (!FApp::IsGame())
	{
		FName TargetText = NAME_None;
		if (ValueOptions.IsEmpty())
		{
			DefaultValue = INDEX_NONE;
			CurrentIdx = INDEX_NONE;
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

void UGCSwitcherSetting::OnPrevClicked()
{
	int32 TargetIdx = CurrentIdx - 1;
	if (TargetIdx < 0)
		TargetIdx = ValueOptions.Num() - 1;
	
	SetValue(TargetIdx);
}

void UGCSwitcherSetting::OnNextClicked()
{
	int32 TargetIdx = CurrentIdx + 1;
	if (TargetIdx >= ValueOptions.Num())
		TargetIdx = 0;
	
	SetValue(TargetIdx);
}