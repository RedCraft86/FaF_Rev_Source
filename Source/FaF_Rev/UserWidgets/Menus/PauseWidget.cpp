// Copyright (C) RedCraft86. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "PauseWidget.h"
#include "Components/TextBlock.h"
#include "GeneralProjectSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/WidgetAnimation.h"
#include "Animation/UMGSequencePlayer.h"
#include "GameSection/GameSectionManager.h"
#include "FRPlayerController.h"
#include "SettingsWidget.h"
#include "SubWidgets.h"
#include "FRGameMode.h"
#include "FRPlayer.h"

UPauseWidgetBase::UPauseWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), ResumeButton(nullptr), SettingsButton(nullptr), CheckpointButton(nullptr)
	, MainMenuButton(nullptr), GameVersionText(nullptr), ScreenFadeAnim(nullptr), PlayerChar(nullptr), SettingsWidget(nullptr)
{
	ZOrder = 96;
	bAutoAdd = false;
}

void UPauseWidgetBase::RemoveWidget(const TFunction<void()>& OnFinished)
{
	GetGameMode<AFRGameModeBase>()->SetGameInputMode(EGameInputMode::GameOnly);
	Super::RemoveWidget(OnFinished);
}

void UPauseWidgetBase::OnResumeClicked()
{
	GetPlayerController<AFRPlayerController>()->SetPauseState(false);
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UPauseWidgetBase::OnSettingsClicked()
{
	SettingsWidget->AddWidget(nullptr);
	GetGameMode<AFRGameModeBase>()->SetGameInputMode(EGameInputMode::GameAndUI, true,
		EMouseLockMode::LockAlways, false, SettingsWidget);
	SetWidgetHidden(true);
}

void UPauseWidgetBase::OnCheckpointClicked()
{
	if (UGameSectionManager* Manager = UGameSectionManager::Get(this))
	{
		Manager->SaveCurrentTime();
	}
	
	FadeScreen([this](){
		UGameplayStatics::OpenLevel(this, *UGameplayStatics::GetCurrentLevelName(this));
	});
}

void UPauseWidgetBase::OnMainMenuClicked()
{
	if (UGameSectionManager* Manager = UGameSectionManager::Get(this))
	{
		Manager->SaveCurrentTime();
	}
	
	FadeScreen([this](){
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, MainMenuLevel);
	});
}

void UPauseWidgetBase::FadeScreen(const TFunction<void()>& Callback)
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
	PlayerChar->FadeToBlack(ScreenFadeAnim->GetEndTime() - ScreenFadeAnim->GetStartTime());
	if (UUMGSequencePlayer* Player = PlayAnimation(ScreenFadeAnim))
	{
		FDelegateHandle Handle = Player->OnSequenceFinishedPlaying().AddLambda([&Handle, Player, this, Callback](UUMGSequencePlayer&){
			PlayerChar->GetGameMode()->SetGameInputMode(EGameInputMode::GameOnly);
			Callback();
			
			Player->OnSequenceFinishedPlaying().Remove(Handle);
		});
	}
}

void UPauseWidgetBase::Return_Implementation(UUserWidget* From)
{
	SetWidgetHidden(false);
	GetGameMode<AFRGameModeBase>()->SetGameInputMode(EGameInputMode::GameAndUI, true,
		EMouseLockMode::LockAlways, false, this);
}

void UPauseWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::Visible);
	GetGameMode<AFRGameModeBase>()->SetGameInputMode(EGameInputMode::GameAndUI, true,
		EMouseLockMode::LockAlways, false, this);
}

void UPauseWidgetBase::InitWidget()
{
	PlayerChar = FRPlayer(this);
	if (const UGeneralProjectSettings* ProjectSettings = GetDefault<UGeneralProjectSettings>())
	{
		GameVersionText->SetText(FText::FromString(FString::Printf(TEXT("Game Version: %s - %s Build"),
			*ProjectSettings->ProjectVersion, LexToString(FApp::GetBuildConfiguration()))));
	}
		
	ResumeButton->OnClicked.AddUObject(this, &UPauseWidgetBase::OnResumeClicked);
	SettingsButton->OnClicked.AddUObject(this, &UPauseWidgetBase::OnSettingsClicked);
	CheckpointButton->OnClicked.AddUObject(this, &UPauseWidgetBase::OnCheckpointClicked);
	MainMenuButton->OnClicked.AddUObject(this, &UPauseWidgetBase::OnMainMenuClicked);

	SettingsWidget = CreateNew<USettingsWidgetBase>(GetPlayerController(), SettingsWidgetClass, {});
	if (SettingsWidget) SettingsWidget->ParentWidget = this;
}
