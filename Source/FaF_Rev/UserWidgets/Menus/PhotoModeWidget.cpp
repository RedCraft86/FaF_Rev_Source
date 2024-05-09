// Copyright (C) RedCraft86. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "PhotoModeWidget.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/SpinBox.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Misc/URLRequestFilter.h"
#include "FRPlayerController.h"
#include "WidgetInterface.h"
#include "FRPlayer.h"
#include "Animation/UMGSequencePlayer.h"

#define POST_PROCESS_SETTINGS PhotoModeActor->CineCamera->PostProcessSettings

#define SETUP_EVENTS(Task) \
	GameTitleCheckBox->OnCheckStateChanged.Task(this, &UPhotoModeWidgetBase::OnGameTitleCheckChanged); \
	ResScaleSpinBox->OnValueChanged.Task(this, &UPhotoModeWidgetBase::OnResScaleChanged); \
	FocalDistSpinBox->OnValueChanged.Task(this, &UPhotoModeWidgetBase::OnFocalDistChanged); \
	FieldOfViewSpinBox->OnValueChanged.Task(this, &UPhotoModeWidgetBase::OnFieldOfViewChanged); \
	ApertureSpinBox->OnValueChanged.Task(this, &UPhotoModeWidgetBase::OnApertureChanged); \
	FilterSpinBox->OnValueChanged.Task(this, &UPhotoModeWidgetBase::OnLUTIntensityChanged); \
	FilterComboBox->OnSelectionChanged.Task(this, &UPhotoModeWidgetBase::OnLUTSelectionChanged); \
	ColorSplitSpinBox->OnValueChanged.Task(this, &UPhotoModeWidgetBase::OnColorSplitChanged); \
	VignetteSpinBox->OnValueChanged.Task(this, &UPhotoModeWidgetBase::OnVignetteChanged); \
	LensFlareSpinBox->OnValueChanged.Task(this, &UPhotoModeWidgetBase::OnLensFlareChanged); \
	BackButton->OnClicked.Task(this, &UPhotoModeWidgetBase::OnBackClicked); \
	FolderButton->OnClicked.Task(this, &UPhotoModeWidgetBase::OnFolderClicked); \
	FileButton->OnClicked.Task(this, &UPhotoModeWidgetBase::OnFolderClicked); \
	SideBarButton->OnClicked.Task(this, &UPhotoModeWidgetBase::OnSidebarClicked); \
	ExitButton->OnClicked.Task(this, &UPhotoModeWidgetBase::OnExitClicked);

UPhotoModeWidgetBase::UPhotoModeWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), GameTitleCheckBox(nullptr), ResScaleSpinBox(nullptr), FocalDistSpinBox(nullptr)
	, FieldOfViewSpinBox(nullptr), ApertureSpinBox(nullptr), FilterSpinBox(nullptr), FilterComboBox(nullptr)
	, ColorSplitSpinBox(nullptr), VignetteSpinBox(nullptr), LensFlareSpinBox(nullptr), CapturedImage(nullptr)
	, BackButton(nullptr), FolderButton(nullptr), FileButton(nullptr), SideBarButton(nullptr), ExitButton(nullptr)
	, SidebarAnim(nullptr), CaptureStartAnim(nullptr), CaptureEndAnim(nullptr), ReturnAnim(nullptr), bIsSidebarOpen(false)
	, ParentWidget(nullptr), CapturedTexture(nullptr), PhotoModeActor(nullptr), Controller(nullptr), PlayerChar(nullptr)
{
	ZOrder = 98;
	bAutoAdd = false;
}

void UPhotoModeWidgetBase::BeginCapture()
{
	if (UUMGSequencePlayer* Player = PlayAnimation(CaptureEndAnim))
	{
		FDelegateHandle Handle = Player->OnSequenceFinishedPlaying().AddLambda([&Handle, Player, this](UUMGSequencePlayer&)
		{
			PhotoModeActor->TakeScreenshot();
			Player->OnSequenceFinishedPlaying().Remove(Handle);
		});
	}
}

void UPhotoModeWidgetBase::SetPhotoModeActor(APhotoModeActor* InActor)
{
	PhotoModeActor = InActor;
	PhotoModeActor->OnCaptured.AddUObject(this, &UPhotoModeWidgetBase::OnPhotoCaptured);
}

void UPhotoModeWidgetBase::OnGameTitleCheckChanged(bool bValue)
{
	PhotoModeActor->bUseWatermark = bValue;
}

void UPhotoModeWidgetBase::OnResScaleChanged(float Value)
{
	PhotoModeActor->ResolutionScale = Value;
}

void UPhotoModeWidgetBase::OnFocalDistChanged(float Value)
{
	PhotoModeActor->CineCamera->FocusSettings.FocusMethod = ECameraFocusMethod::Manual;
	PhotoModeActor->CineCamera->FocusSettings.ManualFocusDistance = Value;
}

void UPhotoModeWidgetBase::OnFieldOfViewChanged(float Value)
{
	PhotoModeActor->CineCamera->SetFieldOfView(Value);
}

void UPhotoModeWidgetBase::OnApertureChanged(float Value)
{
	PhotoModeActor->CineCamera->SetCurrentAperture(Value);
}

void UPhotoModeWidgetBase::OnLUTIntensityChanged(float Value)
{
	POST_PROCESS_SETTINGS.bOverride_ColorGradingIntensity = true;
	POST_PROCESS_SETTINGS.ColorGradingIntensity = Value;
}

void UPhotoModeWidgetBase::OnLUTSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	POST_PROCESS_SETTINGS.bOverride_ColorGradingLUT = SelectedItem != TEXT("None");
	POST_PROCESS_SETTINGS.ColorGradingLUT = GetColorGradingLUT(FilterNamesToID.FindRef(SelectedItem));
}

void UPhotoModeWidgetBase::OnColorSplitChanged(float Value)
{
	POST_PROCESS_SETTINGS.bOverride_SceneFringeIntensity = true;
	POST_PROCESS_SETTINGS.SceneFringeIntensity = Value;
}

void UPhotoModeWidgetBase::OnVignetteChanged(float Value)
{
	POST_PROCESS_SETTINGS.bOverride_VignetteIntensity = true;
	POST_PROCESS_SETTINGS.VignetteIntensity = Value;
}

void UPhotoModeWidgetBase::OnLensFlareChanged(float Value)
{
	POST_PROCESS_SETTINGS.bOverride_LensFlareIntensity = true;
	POST_PROCESS_SETTINGS.LensFlareIntensity = Value;
}

void UPhotoModeWidgetBase::OnBackClicked()
{
	PlayAnimation(ReturnAnim);
}

void UPhotoModeWidgetBase::OnFolderClicked()
{
	const UE::Core::FURLRequestFilter Filter(TEXT("SystemLibrary.LaunchURLFilter"), GEngineIni);
	FPlatformProcess::LaunchURLFiltered(*FString::Printf(TEXT("file://%s"), *APhotoModeActor::GetScreenshotPath()),
		nullptr, nullptr, Filter);
}

void UPhotoModeWidgetBase::OnSidebarClicked()
{
	if (!IsAnimationPlaying(SidebarAnim))
	{
		PlayAnimation(SidebarAnim, 0.0f, 1,
			bIsSidebarOpen ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
		
		bIsSidebarOpen = !bIsSidebarOpen;
	}
}

void UPhotoModeWidgetBase::OnExitClicked()
{
	RemoveWidget(nullptr);
	FSlateBrush Brush = CapturedImage->GetBrush();
	Brush.SetResourceObject(nullptr);
	CapturedImage->SetBrush(Brush);
	CapturedTexture = nullptr;
	
	IWidgetInterface::Return(ParentWidget, this);
}

void UPhotoModeWidgetBase::OnPhotoCaptured(UTexture2D* InTexture)
{
	CapturedTexture = InTexture;
	CapturedImage->SetBrushFromTexture(CapturedTexture);
	PlayAnimation(CaptureEndAnim);
}

void UPhotoModeWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	Controller = FRPlayerController(this);
	PlayerChar = FRPlayer(this);

	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
	CapturedImage->SetDesiredSizeOverride(ViewportSize);
	
	SETUP_EVENTS(RemoveDynamic)
	SETUP_EVENTS(AddDynamic)

	if (!FilterIDs.Contains(NAME_None))
	{
		FilterIDs.Insert(NAME_None, 0);
	}

	FilterNamesToID.Empty();
	FilterComboBox->ClearOptions();
	for (const FName& FilterID : FilterIDs)
	{
		const FString DisplayName = FName::NameToDisplayString(FilterID.ToString(), false);
		FilterNamesToID.Add(DisplayName, FilterID);
		FilterComboBox->AddOption(DisplayName);
	}
	
	GameTitleCheckBox->SetCheckedState(ECheckBoxState::Checked);
	ResScaleSpinBox->SetValue(1.0f);
	FocalDistSpinBox->SetValue(2000.0f);
	ApertureSpinBox->SetValue(4.0f);
	FilterSpinBox->SetValue(1.0f);
	FilterComboBox->SetSelectedIndex(0);
	ColorSplitSpinBox->SetValue(0.0f);
	VignetteSpinBox->SetValue(0.4f);
	LensFlareSpinBox->SetValue(0.0f);
	FieldOfViewSpinBox->SetValue(PlayerChar->PlayerCamera->FieldOfView);
	PhotoModeActor->EnterMode();
}
