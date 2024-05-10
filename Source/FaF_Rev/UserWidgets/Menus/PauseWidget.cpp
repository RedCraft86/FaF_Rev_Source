// Copyright (C) RedCraft86. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "PauseWidget.h"
#include "Components/TextBlock.h"
#include "Animation/UMGSequencePlayer.h"
#include "Animation/WidgetAnimation.h"
#include "GeneralProjectSettings.h"
#include "Kismet/GameplayStatics.h"
#include "FRPlayerController.h"
#include "SettingsWidget.h"
#include "SubWidgets.h"
#include "FRGameMode.h"
#include "FRPlayer.h"

UPauseWidgetBase::UPauseWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), ResumeButton(nullptr), SettingsButton(nullptr), CheckpointButton(nullptr)
	, MainMenuButton(nullptr), GameVersionText(nullptr), ScreenFadeAnim(nullptr), PlayerChar(nullptr), SettingsWidget(nullptr)
{
	ZOrder = 98;
	bAutoAdd = false;
}

void UPauseWidgetBase::OnResumeClicked()
{
	GetPlayerController<AFRPlayerController>()->SetPauseState(false);
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UPauseWidgetBase::OnSettingsClicked()
{
	SettingsWidget->AddWidget(nullptr);
	SetWidgetHidden(true);
}

void UPauseWidgetBase::OnCheckpointClicked()
{
	FadeScreen([this](){
		UGameplayStatics::OpenLevel(this, *UGameplayStatics::GetCurrentLevelName(this));
	});
}

void UPauseWidgetBase::OnMainMenuClicked()
{
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
}

void UPauseWidgetBase::InitWidget()
{
	PlayerChar = FRPlayer(this);
	if (const UGeneralProjectSettings* ProjectSettings = GetDefault<UGeneralProjectSettings>())
	{
		GameVersionText->SetText(FText::FromString(FString::Printf(TEXT("Game Version: %s - %s"),
			*ProjectSettings->ProjectVersion, *ProjectSettings->ProjectDebugTitleInfo.ToString())));
	}
		
	ResumeButton->OnClicked.AddUObject(this, &UPauseWidgetBase::OnResumeClicked);
	SettingsButton->OnClicked.AddUObject(this, &UPauseWidgetBase::OnSettingsClicked);
	CheckpointButton->OnClicked.AddUObject(this, &UPauseWidgetBase::OnCheckpointClicked);
	MainMenuButton->OnClicked.AddUObject(this, &UPauseWidgetBase::OnMainMenuClicked);

	SettingsWidget = CreateWidget<USettingsWidgetBase>(GetPlayerController(), SettingsWidgetClass);
	SettingsWidget->ParentWidget = this;
}
