// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "GCPauseWidget.h"
#include "GCSettingsWidget.h"
#include "GCPhotoModeWidget.h"
#include "GCBugReportWidget.h"
#include "Player/GCPlayerController.h"
#include "UserInterface/GCSubWidgets.h"
#include "UserInterface/Gameplay/GCGameplayWidget.h"
#include "UserInterface/Gameplay/GCNarrativeWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Animation/UMGSequencePlayer.h"
#include "Animation/WidgetAnimation.h"
#include "Framework/GCGameInstance.h"
#include "GeneralProjectSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

UGCPauseWidget::UGCPauseWidget(const FObjectInitializer& ObjectInitializer)
	: UGCUserWidget(ObjectInitializer)
{
	WidgetZOrder = 13;
	bAutoAddToViewport = false;

	ResumeButton = nullptr;
	SettingsButton = nullptr;
	PhotoModeButton = nullptr;
	BugReportButton = nullptr;
	CheckpointButton = nullptr;
	MainMenuButton = nullptr;
	GameVersionText = nullptr;
	ScreenFadeAnim = nullptr;

	MainMenuLevel = nullptr;
	SettingsWidgetClass = NULL;
	PhotoModeWidgetClass = NULL;
	BugReportWidgetClass = NULL;
	SettingsWidget = nullptr;
	PhotoModeWidget = nullptr;
	BugReportWidget = nullptr;
}

void UGCPauseWidget::FadeScreen(const TFunction<void()>& Callback)
{
	const float Duration = ScreenFadeAnim->GetEndTime();
	PlayerController->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, Duration,
		FLinearColor::Black, true, true);

	if (UUMGSequencePlayer* Player = PlayAnimation(ScreenFadeAnim))
	{
		FDelegateHandle Handle;
		Handle = Player->OnSequenceFinishedPlaying().AddLambda([this, Player, &Handle, Callback](UUMGSequencePlayer&){
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
			Callback();

			Player->OnSequenceFinishedPlaying().Remove(Handle);
		});
	}
}

void UGCPauseWidget::InitData(AGCPlayerController* Controller)
{
	Super::InitData(Controller);
	if (const UGeneralProjectSettings* ProjectSettings = GetDefault<UGeneralProjectSettings>())
	{
		GameVersionText->SetText(FText::FromString(FString::Printf(TEXT("Game Version: %s - %s"),
			*ProjectSettings->ProjectVersion, *ProjectSettings->ProjectDebugTitleInfo.ToString())));
	}
	else
	{
		GameVersionText->SetText(FText::FromString(TEXT("Failed to get game version")));
	}
	
	ResumeButton->OnClicked.AddUObject(this, &UGCPauseWidget::OnResumeClicked);
	SettingsButton->OnClicked.AddUObject(this, &UGCPauseWidget::OnSettingsClicked);
	PhotoModeButton->OnClicked.AddUObject(this, &UGCPauseWidget::OnPhotoModeClicked);
	BugReportButton->OnClicked.AddUObject(this, &UGCPauseWidget::OnBugReportClicked);
	CheckpointButton->OnClicked.AddUObject(this, &UGCPauseWidget::OnCheckpointClicked);
	MainMenuButton->OnClicked.AddUObject(this, &UGCPauseWidget::OnMainMenuClicked);

	SettingsWidget = Create<UGCSettingsWidget>(Controller, SettingsWidgetClass);
	SettingsWidget->OnExit.AddUObject(this, &UGCPauseWidget::OnExitExternalMenu);
	
	PhotoModeWidget = Create<UGCPhotoModeWidget>(Controller, PhotoModeWidgetClass);
	PhotoModeWidget->OnExit.AddUObject(this, &UGCPauseWidget::OnExitExternalMenu);
	PhotoModeButton->SetIsEnabled(PhotoModeWidget->CanEnterPhotoMode() &&
		!GetWorld()->GetGameInstance<UGCGameInstance>()->GetViewModeUnlit());

	BugReportWidget = Create<UGCBugReportWidget>(Controller, BugReportWidgetClass);
	BugReportWidget->OnExit.AddUObject(this, &UGCPauseWidget::OnExitExternalMenu);
}

void UGCPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();
	PlayerController->GetUserWidget<UGCGameplayWidget>()->SetWidgetHidden(true);
	PlayerController->GetUserWidget<UGCNarrativeWidget>()->SetWidgetHidden(true);
}

void UGCPauseWidget::NativeDestruct()
{
	Super::NativeDestruct();
	PlayerController->GetUserWidget<UGCGameplayWidget>()->SetWidgetHidden(false);
	PlayerController->GetUserWidget<UGCNarrativeWidget>()->SetWidgetHidden(false);
}

void UGCPauseWidget::OnExitExternalMenu()
{
	PhotoModeButton->SetIsEnabled(PhotoModeWidget->CanEnterPhotoMode() &&
		!GetWorld()->GetGameInstance<UGCGameInstance>()->GetViewModeUnlit());
	
	SetWidgetHidden(false);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGCPauseWidget::OnResumeClicked()
{
	PlayerController->UnpauseGame();
}

void UGCPauseWidget::OnSettingsClicked()
{
	SettingsWidget->RefreshDisplay();
	SettingsWidget->AddWidget(nullptr, true);
	SetWidgetHidden(true);
}

void UGCPauseWidget::OnPhotoModeClicked()
{
	PhotoModeWidget->AddWidget(nullptr, true);
	SetWidgetHidden(true);
}

void UGCPauseWidget::OnBugReportClicked()
{
	BugReportWidget->AddWidget(nullptr, true);
	SetWidgetHidden(true);
}

void UGCPauseWidget::OnCheckpointClicked()
{
	FadeScreen([this](){
		UGameplayStatics::OpenLevel(this, *UGameplayStatics::GetCurrentLevelName(this));
	});
}

void UGCPauseWidget::OnMainMenuClicked()
{
	FadeScreen([this](){
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, MainMenuLevel);
	});
}
