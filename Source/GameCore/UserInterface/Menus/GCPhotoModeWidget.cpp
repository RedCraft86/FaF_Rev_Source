// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
// ReSharper disable CppParameterMayBeConst

#include "GCPhotoModeWidget.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/SpinBox.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Animation/UMGSequencePlayer.h"
#include "PhotoMode/GCPhotoModeActor.h"
#include "Player/GCPlayerController.h"
#include "Player/GCPlayerCharacter.h"
#include "Misc/URLRequestFilter.h"

#define POST_PROCESS_SETTINGS PhotoModeActor->CineCamera->PostProcessSettings

UGCPhotoModeWidget::UGCPhotoModeWidget(const FObjectInitializer& ObjectInitializer)
	: UGCUserWidget(ObjectInitializer)
{
	WidgetZOrder = 13;
	bAutoAddToViewport = false;

	GameTitleCheckBox = nullptr;
	ResScaleSpinBox = nullptr;
	FocalDistSpinBox = nullptr;
	FieldOfViewSpinBox = nullptr;
	ApertureSpinBox = nullptr;
	FilterSpinBox = nullptr;
	FilterComboBox = nullptr;
	ColorSplitSpinBox = nullptr;
	VignetteSpinBox = nullptr;
	LensFlareSpinBox = nullptr;
	CapturedImage = nullptr;
	BackButton = nullptr;
	FolderButton = nullptr;
	FileButton = nullptr;
	SideBarButton = nullptr;
	ExitButton = nullptr;
	ScreenSwapAnim = nullptr;
	SidebarAnim = nullptr;
	CaptureStartAnim = nullptr;
	CaptureEndAnim = nullptr;

	PhotoModeActor = nullptr;
	bIsSidebarOpen = true;
	CapturedDelay = 0.0f;
	TempImg = nullptr;
}

bool UGCPhotoModeWidget::CanEnterPhotoMode() const
{
	return IsValid(PhotoModeActor);
}

void UGCPhotoModeWidget::OnBeginCapture()
{
	PlayAnimation(CaptureStartAnim);
}

void UGCPhotoModeWidget::OnShotCaptured(UTexture2D* Image)
{
	TempImg = Image;
	CapturedDelay = 0.25f;
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UGCPhotoModeWidget::InitData(AGCPlayerController* Controller)
{
	Super::InitData(Controller);

	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
	CapturedImage->SetDesiredSizeOverride(ViewportSize);

	PhotoModeActor = Controller->GetPhotoModeActor();
	if (PhotoModeActor)
	{
		PhotoModeActor->OnCaptureStarted.AddUObject(this, &UGCPhotoModeWidget::OnBeginCapture);
		PhotoModeActor->OnCaptureFinished.AddUObject(this, &UGCPhotoModeWidget::OnShotCaptured);
	}

	GameTitleCheckBox->OnCheckStateChanged.AddDynamic(this, &UGCPhotoModeWidget::OnGameTitleCheckChanged);
	ResScaleSpinBox->OnValueChanged.AddDynamic(this, &UGCPhotoModeWidget::OnResScaleChanged);
	FocalDistSpinBox->OnValueChanged.AddDynamic(this, &UGCPhotoModeWidget::OnFocalDistChanged);
	FieldOfViewSpinBox->OnValueChanged.AddDynamic(this, &UGCPhotoModeWidget::OnFieldOfViewChanged);
	ApertureSpinBox->OnValueChanged.AddDynamic(this, &UGCPhotoModeWidget::OnApertureChanged);
	FilterSpinBox->OnValueChanged.AddDynamic(this, &UGCPhotoModeWidget::OnLUTIntensityChanged);
	FilterComboBox->OnSelectionChanged.AddDynamic(this, &UGCPhotoModeWidget::OnLUTSelectionChanged);
	ColorSplitSpinBox->OnValueChanged.AddDynamic(this, &UGCPhotoModeWidget::OnColorSplitChanged);
	VignetteSpinBox->OnValueChanged.AddDynamic(this, &UGCPhotoModeWidget::OnVignetteChanged);
	LensFlareSpinBox->OnValueChanged.AddDynamic(this, &UGCPhotoModeWidget::OnLensFlareChanged);
	BackButton->OnClicked.AddDynamic(this, &UGCPhotoModeWidget::OnBackClicked);
	FolderButton->OnClicked.AddDynamic(this, &UGCPhotoModeWidget::OnFolderClicked);
	FileButton->OnClicked.AddDynamic(this, &UGCPhotoModeWidget::OnFolderClicked);
	SideBarButton->OnClicked.AddDynamic(this, &UGCPhotoModeWidget::OnSidebarClicked);
	ExitButton->OnClicked.AddDynamic(this, &UGCPhotoModeWidget::OnExitClicked);
}

void UGCPhotoModeWidget::NativeTick(const FGeometry& InGeometry, float InDeltaTime)
{
	Super::NativeTick(InGeometry, InDeltaTime);
	if (CapturedDelay > 0.0f)
	{
		CapturedDelay -= InDeltaTime;
		if (CapturedDelay <= 0.0f)
		{
			PlayAnimation(CaptureEndAnim);
			PlayAnimation(ScreenSwapAnim, 0.38f);
			CapturedImage->SetBrushFromTexture(TempImg, true);
			SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			TempImg = nullptr;
		}
	}
}

void UGCPhotoModeWidget::NativeConstruct()
{
	Super::NativeConstruct();

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
	FieldOfViewSpinBox->SetValue(PlayerController->GetPlayer()->PlayerCamera->FieldOfView);
	PhotoModeActor->StartPhotoMode(PlayerController);
}

void UGCPhotoModeWidget::OnGameTitleCheckChanged(bool bValue)
{
	PhotoModeActor->bWatermarkImage = bValue;
}

void UGCPhotoModeWidget::OnResScaleChanged(float Value)
{
	PhotoModeActor->ResolutionScale = Value;
}

void UGCPhotoModeWidget::OnFocalDistChanged(float Value)
{
	PhotoModeActor->CineCamera->FocusSettings.FocusMethod = ECameraFocusMethod::Manual;
	PhotoModeActor->CineCamera->FocusSettings.ManualFocusDistance = Value;
}

void UGCPhotoModeWidget::OnFieldOfViewChanged(float Value)
{
	PhotoModeActor->CineCamera->SetFieldOfView(Value);
}

void UGCPhotoModeWidget::OnApertureChanged(float Value)
{
	PhotoModeActor->CineCamera->SetCurrentAperture(Value);
}

void UGCPhotoModeWidget::OnLUTIntensityChanged(float Value)
{
	POST_PROCESS_SETTINGS.bOverride_ColorGradingIntensity = true;
	POST_PROCESS_SETTINGS.ColorGradingIntensity = Value;
}

void UGCPhotoModeWidget::OnLUTSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	POST_PROCESS_SETTINGS.bOverride_ColorGradingLUT = true;
	POST_PROCESS_SETTINGS.ColorGradingLUT = GetColorGradingLUT(FilterNamesToID.FindRef(SelectedItem));
}

void UGCPhotoModeWidget::OnColorSplitChanged(float Value)
{
	POST_PROCESS_SETTINGS.bOverride_SceneFringeIntensity = true;
	POST_PROCESS_SETTINGS.SceneFringeIntensity = Value;
}

void UGCPhotoModeWidget::OnVignetteChanged(float Value)
{
	POST_PROCESS_SETTINGS.bOverride_VignetteIntensity = true;
	POST_PROCESS_SETTINGS.VignetteIntensity = Value;
}

void UGCPhotoModeWidget::OnLensFlareChanged(float Value)
{
	POST_PROCESS_SETTINGS.bOverride_LensFlareIntensity = true;
	POST_PROCESS_SETTINGS.LensFlareIntensity = Value;
}

void UGCPhotoModeWidget::OnBackClicked()
{
	if (UUMGSequencePlayer* Player = PlayAnimation(ScreenSwapAnim, 0.0f, 1,
		EUMGSequencePlayMode::Reverse, 2.0f))
	{
		FDelegateHandle Handle;
		Handle = Player->OnSequenceFinishedPlaying().AddLambda([this, Player, &Handle](UUMGSequencePlayer&){
			PhotoModeActor->bWaitForUI = false;
			
			Player->OnSequenceFinishedPlaying().Remove(Handle);
		});
	}
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UGCPhotoModeWidget::OnFolderClicked()
{
	const UE::Core::FURLRequestFilter Filter(TEXT("SystemLibrary.LaunchURLFilter"), GEngineIni);
	FPlatformProcess::LaunchURLFiltered(*FString::Printf(TEXT("file://%s"), *AGCPhotoModeActor::GetScreenshotPath()),
		nullptr, nullptr, Filter);
}

void UGCPhotoModeWidget::OnSidebarClicked()
{
	if (!IsAnimationPlaying(SidebarAnim))
	{
		PlayAnimation(SidebarAnim, 0.0f, 1,
			bIsSidebarOpen ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
		
		bIsSidebarOpen = !bIsSidebarOpen;
	}
}

void UGCPhotoModeWidget::OnExitClicked()
{
	RemoveWidget(nullptr, true);
	CapturedImage->SetBrushFromTexture(nullptr, true);
	PhotoModeActor->FinishPhotoMode(PlayerController);
	OnExit.Broadcast();
	OnExitBP.Broadcast();
}
