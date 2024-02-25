// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCMessageWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Player/GCPlayerController.h"
#include "Achievement/GCAchievementManager.h"
#include "Components/HorizontalBoxSlot.h"
#include "Inventory/GCInventoryManager.h"

UGCKeyHintWidget::UGCKeyHintWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	LabelText = nullptr;
	IconsBox = nullptr;
}

void UGCKeyHintWidget::SetData(const FText& InLabel, const TArray<UTexture2D*> InKeys)
{
	LabelText->SetText(InLabel);
	IconsBox->ClearChildren();
	EObjectFlags NewObjectFlags = RF_Transactional;
	if (HasAnyFlags(RF_Transient))
	{
		NewObjectFlags |= RF_Transient;
	}
	for (UTexture2D* Image : InKeys)
	{
		if (!Image) continue;
		UImage* IconImage = NewObject<UImage>(this, NAME_None, NewObjectFlags);
		IconImage->SetBrushFromTexture(Image);
					
		UHorizontalBoxSlot* IconSlot = Cast<UHorizontalBoxSlot>(IconsBox->AddChild(IconImage));
		IconSlot->SetHorizontalAlignment(HAlign_Center);
		IconSlot->SetVerticalAlignment(VAlign_Center);
	}
}

UGCMessageWidget::UGCMessageWidget(const FObjectInitializer& ObjectInitializer)
	: UGCUserWidget(ObjectInitializer)
{
	SubtitleName = nullptr;
	AchievementIcon = nullptr;
	AchievementName = nullptr;
	KeyHintBox = nullptr;
	SubtitleAnim = nullptr;
	NoticeAnim = nullptr;
	AchievementAnim = nullptr;
	KeyHintAnim = nullptr;
	
	DefaultAchievementIcon = nullptr;
	KeyHintWidgetClass = NULL;
	
	AchievementManager = nullptr;
	InventoryManager = nullptr;
	bHasPlayedHintAnim = false;
	LastNoticeObject = nullptr;
	LastSubtitleObject = nullptr;
}

void UGCMessageWidget::QueueNoticeText(const FText InText, const float InTime, const bool bOverrideQueue)
{
	FGCNoticeData Data;
	Data.TextBasic = InText;
	Data.DisplayTime = InTime;
	LastNoticeObject = nullptr;
	QueueNotice(Data, bOverrideQueue);
}

void UGCMessageWidget::QueueNotice(const FGCNoticeData& InData, const bool bOverrideQueue)
{
	if (Notice.GetTextString() == InData.GetTextString()) return;
	if (const FGCNoticeData* PeekVal = NoticeQueue.Peek())
	{
		if (PeekVal->GetTextString() == InData.GetTextString()) return;
	}
	
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
	Data.TextBasic = InText;
	Data.DisplayTime = InTime;
	LastSubtitleObject = nullptr;
	QueueSubtitle(Data, bOverrideQueue);
}

void UGCMessageWidget::QueueSubtitle(const FGCSubtitleData& InData, const bool bOverrideQueue)
{
	if (Subtitle.GetTextString() == InData.GetTextString()) return;
	if (const FGCSubtitleData* PeekVal = SubtitleQueue.Peek())
	{
		if (PeekVal->Name.ToString() == InData.Name.ToString() && PeekVal->GetTextString() == InData.GetTextString()) return;
	}
	
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

void UGCMessageWidget::RemoveKeyHint(const FName InID)
{
	if (InID.IsNone()) return;
	if (UGCKeyHintWidget* Widget = KeyHints.FindRef(InID))
	{
		KeyHints.Remove(InID);
		if (KeyHints.IsEmpty())
		{
			PlayAnimationReverse(KeyHintAnim);
			bHasPlayedHintAnim = false;
		
			FTimerHandle Handle;
			GetWorld()->GetTimerManager().SetTimer(Handle, [Widget]()
			{
				Widget->RemoveFromParent();
			}, 0.5f, false);
		}
		else
		{
			Widget->RemoveFromParent();
		}
	}
}

void UGCMessageWidget::AddKeyHint(const FName InID, const FText InLabel, const TArray<UTexture2D*> InKeys)
{
	if (InID.IsNone() || InLabel.IsEmptyOrWhitespace()
		|| InKeys.IsEmpty() || !KeyHintWidgetClass) return;
	
	RemoveKeyHint(InID);
	if (UGCKeyHintWidget* Widget = CreateWidget<UGCKeyHintWidget>(this, KeyHintWidgetClass))
	{
		Widget->SetData(InLabel, InKeys);
		KeyHintBox->AddChild(Widget);
		KeyHints.Add(InID, Widget);
		if (!KeyHints.IsEmpty() && !bHasPlayedHintAnim)
		{
			bHasPlayedHintAnim = true;
			PlayAnimation(KeyHintAnim);
		}
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
	if (NoticeQueue.Dequeue(Notice))
	{
		PlayAnimation(NoticeAnim);
		SetNoticeText(Notice.GetText());

		GetWorld()->GetTimerManager().SetTimer(NoticeTimer, this, &UGCMessageWidget::UpdateNotice,
			Notice.GetDisplayTime(), false);
	}
	else
	{
		Notice = {};
		NoticeTimer.Invalidate();
		PlayAnimationReverse(NoticeAnim);
		LastNoticeObject = nullptr;
	}
}

void UGCMessageWidget::UpdateSubtitle()
{
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
		Subtitle = {};
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
