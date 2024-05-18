// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRGameMode.h"
#include "NarrativeComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Menus/InventoryWidget.h"
#include "Core/MessageWidget.h"
#include "FRPlayerController.h"
#include "FRGameState.h"
#include "FRPlayer.h"


AFRGameModeBase::AFRGameModeBase()
	: PlayerController(nullptr), PlayerCharacter(nullptr)
{
	PlayerControllerClass = AFRPlayerController::StaticClass();
	DefaultPawnClass = AFRPlayerBase::StaticClass();
	GameStateClass = AFRGameStateBase::StaticClass();
	
	Narrative = CreateDefaultSubobject<UNarrativeComponent>("Narrative");
	Inventory = CreateDefaultSubobject<UInventoryComponent>("Inventory");
}

void AFRGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	Inventory->InventoryWidget = GetWidget<UInventoryWidgetBase>();
}

void AFRGameModeBase::QueueSmallNotice(const FSimpleNoticeData& NoticeData, const bool bResetQueue)
{
	if (UMessageWidgetBase* Widget = GetWidget<UMessageWidgetBase>())
	{
		Widget->QueueSmallNotice(NoticeData, bResetQueue);
	}
}

void AFRGameModeBase::QueueLargeNotice(const FSimpleNoticeData& NoticeData, const bool bResetQueue)
{
	if (UMessageWidgetBase* Widget = GetWidget<UMessageWidgetBase>())
	{
		Widget->QueueLargeNotice(NoticeData, bResetQueue);
	}
}

void AFRGameModeBase::QueueSubtitles(const TArray<FSimpleSubtitleData>& Subtitles)
{
	if (UMessageWidgetBase* Widget = GetWidget<UMessageWidgetBase>())
	{
		Widget->QueueSubtitles(Subtitles);
	}
}

void AFRGameModeBase::QueueSubtitle(const FSimpleSubtitleData& SubtitleData)
{
	if (UMessageWidgetBase* Widget = GetWidget<UMessageWidgetBase>())
	{
		Widget->QueueSubtitle(SubtitleData);
	}
}
