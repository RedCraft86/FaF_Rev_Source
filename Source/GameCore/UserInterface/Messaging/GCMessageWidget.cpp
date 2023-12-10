// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "GCMessageWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Player/GCPlayerController.h"
#include "Achievement/GCAchievementManager.h"
#include "Inventory/GCInventoryManager.h"

UGCMessageWidget::UGCMessageWidget(const FObjectInitializer& ObjectInitializer)
	: UGCUserWidget(ObjectInitializer)
{
	WidgetZOrder = 14;
	bAutoAddToViewport = true;
	
	SubtitleName = nullptr;
	AchievementIcon = nullptr;
	AchievementName = nullptr;
	SubtitleAnim = nullptr;
	NoticeAnim = nullptr;
	AchievementAnim = nullptr;
	
	DefaultAchievementIcon = nullptr;
	AchievementManager = nullptr;
	InventoryManager = nullptr;
	
	LastNoticeObject = nullptr;
	LastSubtitleObject = nullptr;
}

void UGCMessageWidget::QueueNoticeText(const FText InText, const float InTime, const bool bOverrideQueue)
{
	FGCNoticeData Data;
	Data.Text.Text = InText;
	Data.DisplayTime = InTime;
	QueueNotice(Data, bOverrideQueue);
}

void UGCMessageWidget::QueueNotice(const FGCNoticeData& InData, const bool bOverrideQueue)
{
	LastNoticeObject = nullptr;
	if (bOverrideQueue)
	{
		NoticeQueue.Empty();
		GetWorld()->GetTimerManager().ClearTimer(NoticeTimer);
	}

	NoticeQueue.Enqueue(InData);
	if (!NoticeTimer.IsValid())
	{
		UpdateNotice();
	}
}

void UGCMessageWidget::QueueSubtitleText(const FText InText, const float InTime, const bool bOverrideQueue)
{
	FGCSubtitleData Data;
	Data.Text.Text = InText;
	Data.DisplayTime = InTime;
	QueueSubtitle(Data, bOverrideQueue);
}

void UGCMessageWidget::QueueSubtitle(const FGCSubtitleData& InData, const bool bOverrideQueue)
{
	LastNoticeObject = nullptr;
	if (bOverrideQueue)
	{
		SubtitleQueue.Empty();
		GetWorld()->GetTimerManager().ClearTimer(SubtitleTimer);
	}
	
	SubtitleQueue.Enqueue(InData);
	if (!SubtitleTimer.IsValid())
	{
		UpdateSubtitle();
	}
}

void UGCMessageWidget::QueueNoticeByObject(const FGCNoticeData& InData, const UObject* InObject)
{
	QueueNotice(InData, InObject ? InObject != LastNoticeObject : false);
	LastNoticeObject = InObject;
}

void UGCMessageWidget::QueueSubtitleByObject(const FGCSubtitleData& InData, const UObject* InObject)
{
	QueueSubtitle(InData, InObject ? InObject != LastSubtitleObject : false);
	LastSubtitleObject = InObject;
}

void UGCMessageWidget::UpdateNotice()
{
	FGCNoticeData Notice;
	if (NoticeQueue.Dequeue(Notice))
	{
		PlayAnimation(NoticeAnim);
		SetNoticeText(Notice.GetText());

		GetWorld()->GetTimerManager().SetTimer(NoticeTimer, this, &UGCMessageWidget::UpdateNotice,
			Notice.GetDisplayTime(), false);
	}
	else
	{
		NoticeTimer.Invalidate();
		PlayAnimationReverse(NoticeAnim);
		LastNoticeObject = nullptr;
	}
}

void UGCMessageWidget::UpdateSubtitle()
{
	FGCSubtitleData Subtitle;
	if (SubtitleQueue.Dequeue(Subtitle))
	{
		PlayAnimation(SubtitleAnim);
		SubtitleName->SetText(Subtitle.Name);
		SetSubtitleText(Subtitle.GetText());

		GetWorld()->GetTimerManager().SetTimer(SubtitleTimer, this, &UGCMessageWidget::UpdateSubtitle,
			Subtitle.GetDisplayTime(), false);
	}
	else
	{
		SubtitleTimer.Invalidate();
		PlayAnimationReverse(SubtitleAnim);
		LastSubtitleObject = nullptr;
	}
}

void UGCMessageWidget::OnAchievementUnlocked(const FGCAchievementID& ID)
{
	const FGCAchievementData Data = UGCAchievementManager::GetAchievementData(ID);
	if (Data.IsValidData())
	{
		PLAY_FIRST_FRAME(AchievementAnim)
		
		AchievementIcon->SetBrushFromTexture(Data.IconImage.IsNull() ?
			DefaultAchievementIcon : Data.IconImage.LoadSynchronous(), false);
		
		AchievementName->SetText(Data.DisplayName);
		PlayAnimation(AchievementAnim);
	}
}

void UGCMessageWidget::OnInventoryItemOverflow(const FGCInvItemID& ID, const uint8 Overflow)
{
	const FGCInvItemData Data = UGCInventoryManager::GetItemData(ID);
	if (Data.IsValidData())
	{
		const FString Msg(FString::Printf(TEXT("Exceeded limit for %s. %d items will be left behind."),
			*Data.DisplayName.ToString(), Overflow));

		QueueNotice(FGCNoticeData{FText::FromString(Msg), 2.0f}, true);
	}
}

void UGCMessageWidget::OnInventoryItemMissing(const FGCInvItemID& ID, const uint8 Missing)
{
	const FGCInvItemData Data = UGCInventoryManager::GetItemData(ID);
	if (Data.IsValidData())
	{
		const FString Msg(FString::Printf(TEXT("Not enough %s(s). %d items still needed."),
			*Data.DisplayName.ToString(), Missing));

		QueueNotice(FGCNoticeData{FText::FromString(Msg), 2.0f}, true);
	}
}

void UGCMessageWidget::InitData(AGCPlayerController* Controller)
{
	Super::InitData(Controller);
	AchievementManager = Controller->GetAchievementManager();
	InventoryManager = Controller->GetInventoryManager();
}

void UGCMessageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetNoticeText({});
	SetSubtitleText({});
	SubtitleName->SetText(FText::GetEmpty());
	AchievementIcon->SetBrushFromTexture(DefaultAchievementIcon, false);
	AchievementName->SetText(FText::GetEmpty());

	PLAY_FIRST_FRAME(NoticeAnim)
	PLAY_FIRST_FRAME(SubtitleAnim)
	PLAY_FIRST_FRAME(AchievementAnim)

	if (AchievementManager)
	{
		AchievementManager->OnAchievementComplete.RemoveAll(this);
		AchievementManager->OnAchievementComplete.AddUObject(this, &UGCMessageWidget::OnAchievementUnlocked);
	}

	if (InventoryManager)
	{
		InventoryManager->OnItemOverflow.RemoveAll(this);
		InventoryManager->OnItemMissing.RemoveAll(this);
		InventoryManager->OnItemOverflow.AddUObject(this, &UGCMessageWidget::OnInventoryItemOverflow);
		InventoryManager->OnItemMissing.AddUObject(this, &UGCMessageWidget::OnInventoryItemMissing);
	}
}

void UGCMessageWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (AchievementManager)
	{
		AchievementManager->OnAchievementComplete.RemoveAll(this);
	}

	if (InventoryManager)
	{
		InventoryManager->OnItemOverflow.RemoveAll(this);
		InventoryManager->OnItemMissing.RemoveAll(this);
	}
}