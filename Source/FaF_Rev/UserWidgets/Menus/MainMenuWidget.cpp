// Copyright (C) RedCraft86. All Rights Reserved.

#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "GeneralProjectSettings.h"
#include "Animation/UMGSequencePlayer.h"
#include "SaveSystem/SaveSubsystem.h"
#include "SettingsWidget.h"
#include "PhaseMapWidget.h"
#include "FRGameMode.h"

UMainMenuWidgetBase::UMainMenuWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), PlayButton(nullptr), SettingsButton(nullptr)
	, QuitButton(nullptr), PhaseMapButton(nullptr), ExtrasButton(nullptr), GameVersionText(nullptr)
	, GameplayLevel(nullptr), ExtrasLevel(nullptr), SettingsWidgetClass(nullptr), PhaseMapWidgetClass(nullptr)
{
	ZOrder = 90;
	bAutoAdd = true;
}

void UMainMenuWidgetBase::AddWidget(const TFunction<void()>& OnFinished)
{
	//Super::AddWidget(OnFinished);

	if (IsInViewport()) return;

	AddToViewport(ZOrder);
	if (FadeAnim)
	{
		if (UUMGSequencePlayer* Player = PlayAnimationForward(FadeAnim, 0.5f))
		{
			if (!OnFinished) return;
			Player->OnSequenceFinishedPlaying().Clear();
			Player->OnSequenceFinishedPlaying().AddLambda([OnFinished](UUMGSequencePlayer&)
			{
				if (OnFinished) OnFinished();
			});
		}
	}
	else if (OnFinished)
	{
		OnFinished();
	}
}

void UMainMenuWidgetBase::RemoveWidget(const TFunction<void()>& OnFinished)
{
	GetGameMode<AFRGameModeBase>()->SetGameInputMode(EGameInputMode::GameOnly);
	Super::RemoveWidget(OnFinished);
}

void UMainMenuWidgetBase::OnPlayClicked()
{
	FadeOut();
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, [this]()
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, GameplayLevel);
	}, 2.0f, false);
}

void UMainMenuWidgetBase::OnSettingsClicked()
{
	SettingsWidget->AddWidget(nullptr);
	GetGameMode<AFRGameModeBase>()->SetGameInputMode(EGameInputMode::GameAndUI, true,
		EMouseLockMode::LockAlways, false, SettingsWidget);
	SetWidgetHidden(true);
}

void UMainMenuWidgetBase::OnQuitClicked()
{
	FadeOut();
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, [this]()
	{
		UKismetSystemLibrary::QuitGame(this, nullptr,
			EQuitPreference::Quit, false);
	}, 2.0f, false);
}

void UMainMenuWidgetBase::OnPhaseMapClicked()
{
	PhaseMapWidget->AddWidget(nullptr);
	GetGameMode<AFRGameModeBase>()->SetGameInputMode(EGameInputMode::GameAndUI, true,
		EMouseLockMode::LockAlways, false, PhaseMapWidget);
	SetWidgetHidden(true);
}

void UMainMenuWidgetBase::OnExtrasClicked()
{
	FadeOut();
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, [this]()
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, ExtrasLevel);
	}, 2.0f, false);
}

void UMainMenuWidgetBase::FadeOut()
{
	RemoveWidget(nullptr);
	GetPlayerController()->PlayerCameraManager->StartCameraFade(0.0f, 1.0f,
		1.0f, FLinearColor::Black, true, true);
}

void UMainMenuWidgetBase::Return_Implementation(UUserWidget* From)
{
	SetWidgetHidden(false);
	GetGameMode<AFRGameModeBase>()->SetGameInputMode(EGameInputMode::GameAndUI, true,
		EMouseLockMode::LockAlways, false, this);
}

void UMainMenuWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::Visible);
	GetGameMode<AFRGameModeBase>()->SetGameInputMode(EGameInputMode::GameAndUI, true,
		EMouseLockMode::LockAlways, false, this);
}

void UMainMenuWidgetBase::InitWidget()
{
	Super::InitWidget();
	if (const UGeneralProjectSettings* ProjectSettings = GetDefault<UGeneralProjectSettings>())
	{
		GameVersionText->SetText(FText::FromString(FString::Printf(TEXT("Game Version: %s - %s Build"),
			*ProjectSettings->ProjectVersion, LexToString(FApp::GetBuildConfiguration()))));
	}
		
	PlayButton->OnClicked.AddDynamic(this, &UMainMenuWidgetBase::OnPlayClicked);
	SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidgetBase::OnSettingsClicked);
	QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidgetBase::OnQuitClicked);
	PhaseMapButton->OnClicked.AddDynamic(this, &UMainMenuWidgetBase::OnPhaseMapClicked);
	ExtrasButton->OnClicked.AddDynamic(this, &UMainMenuWidgetBase::OnExtrasClicked);

	if (const USaveSubsystem* SaveManager = USaveSubsystem::Get(this))
	{
		PhaseMapButton->SetVisibility(SaveManager->HasAnyEnding()
			? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		
		ExtrasButton->SetVisibility(PhaseMapButton->GetVisibility());
	}

	SettingsWidget = CreateNew<USettingsWidgetBase>(GetPlayerController(), SettingsWidgetClass, {});
	if (SettingsWidget) SettingsWidget->ParentWidget = this;

	PhaseMapWidget = CreateNew<UPhaseMapWidgetBase>(GetPlayerController(), PhaseMapWidgetClass, {});
	if (PhaseMapWidget) PhaseMapWidget->ParentWidget = this;
}
