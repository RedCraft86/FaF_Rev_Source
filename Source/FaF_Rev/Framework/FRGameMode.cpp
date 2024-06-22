// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRGameMode.h"
#include "NarrativeComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Menus/InventoryWidget.h"
#include "Screens/GuideWidget.h"
#include "Core/MessageWidget.h"
#include "FRPlayerController.h"
#include "FRPlayer.h"

AFRGameModeBase::AFRGameModeBase()
	: PlayerController(nullptr), PlayerCharacter(nullptr)
{
	PlayerControllerClass = AFRPlayerController::StaticClass();
	DefaultPawnClass = AFRPlayerBase::StaticClass();
	
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

void AFRGameModeBase::QueueSubtitle(const FSimpleSubtitleData& SubtitleData, const bool bOverride)
{
	if (UMessageWidgetBase* Widget = GetWidget<UMessageWidgetBase>())
	{
		Widget->QueueSubtitle(SubtitleData, bOverride);
	}
}

void AFRGameModeBase::QueueSubtitles(const TArray<FSimpleSubtitleData>& Subtitles, const bool bOverride)
{
	if (UMessageWidgetBase* Widget = GetWidget<UMessageWidgetBase>())
	{
		Widget->QueueSubtitles(Subtitles, bOverride);
	}
}

void AFRGameModeBase::QueueGuidePage(const FGuideBookPageID PageID)
{
	if (UGuideWidgetBase* Widget = GetWidget<UGuideWidgetBase>())
	{
		Widget->QueuePage(PageID);
	}
}

void AFRGameModeBase::QueueGuidePages(const TArray<FGuideBookPageID>& PageIDs)
{
	if (UGuideWidgetBase* Widget = GetWidget<UGuideWidgetBase>())
	{
		Widget->QueuePages(PageIDs);
	}
}
