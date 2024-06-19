// Copyright (C) RedCraft86. All Rights Reserved.

#include "MessageWidget.h"
#include "ExprTextBlock.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

UMessageWidgetBase::UMessageWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), SmallNoticeText(nullptr), LargeNoticeText(nullptr)
	, SubtitleLineText(nullptr), SubtitleNameText(nullptr), SmallNoticeAnim(nullptr)
	, LargeNoticeAnim(nullptr), SubtitleAnim(nullptr), SubtitlePauseFade(nullptr)
	, bAutoHidden(false), HideCheckTime(0.0f), WorldSettings(nullptr)
{
	ZOrder = 95;
	bAutoAdd = true;
}

void UMessageWidgetBase::QueueSmallNotice(const FSimpleNoticeData& NoticeData, const bool bResetQueue)
{
	if (!NoticeData.IsValidData()) return;
	if (!SmallNoticeQueue.IsEmpty() && GetTypeHash(*SmallNoticeQueue.Peek()) == GetTypeHash(NoticeData)) return;
	if (bResetQueue)
	{
		GetWorld()->GetTimerManager().ClearTimer(SmallNoticeTimer);
		SmallNoticeQueue.Empty();
	}

	SmallNoticeQueue.Enqueue(NoticeData);
	if (!SmallNoticeTimer.IsValid() || !GetWorld()->GetTimerManager().IsTimerActive(SmallNoticeTimer))
	{
		UpdateSmallNotice();
	}
}

void UMessageWidgetBase::QueueLargeNotice(const FSimpleNoticeData& NoticeData, const bool bResetQueue)
{
	if (!NoticeData.IsValidData()) return;
	if (!LargeNoticeQueue.IsEmpty() && GetTypeHash(*LargeNoticeQueue.Peek()) == GetTypeHash(NoticeData)) return;
	if (bResetQueue)
	{
		GetWorld()->GetTimerManager().ClearTimer(LargeNoticeTimer);
		LargeNoticeQueue.Empty();
	}

	LargeNoticeQueue.Enqueue(NoticeData);
	if (!LargeNoticeTimer.IsValid() || !GetWorld()->GetTimerManager().IsTimerActive(LargeNoticeTimer))
	{
		UpdateLargeNotice();
	}
}

void UMessageWidgetBase::QueueSubtitles(const TArray<FSimpleSubtitleData>& Subtitles, const bool bOverride)
{
	int i = 0;
	for (const FSimpleSubtitleData& Subtitle : Subtitles)
	{
		QueueSubtitle(Subtitle, i == 0 && bOverride);
		i++;
	}
}

void UMessageWidgetBase::QueueSubtitle(const FSimpleSubtitleData& SubtitleData, const bool bOverride)
{
	if (!SubtitleData.IsValidData()) return;

	if (bOverride)
	{
		SubtitleQueue.Empty();
		GetWorld()->GetTimerManager().ClearTimer(SubtitleTimer);
	}
	
	SubtitleQueue.Enqueue(SubtitleData);
	if (!SubtitleTimer.IsValid() || !GetWorld()->GetTimerManager().IsTimerActive(SubtitleTimer))
	{
		UpdateSubtitle();
	}
}

void UMessageWidgetBase::UpdateSmallNotice()
{
	FSimpleNoticeData Data;
	if (SmallNoticeQueue.Dequeue(Data))
	{
		if (Data.IsValidData())
		{
			PlayAnimation(SmallNoticeAnim);
			SmallNoticeText->SetText(Data.Message, true);
			GetWorld()->GetTimerManager().SetTimer(SmallNoticeTimer,
				this, &UMessageWidgetBase::UpdateSmallNotice,
				Data.CalcDisplayTime(), false);
		}
		else
		{
			UpdateSmallNotice();
		}
	}
	else
	{
		PlayAnimationReverse(SmallNoticeAnim);
		SmallNoticeTimer.Invalidate();
	}
}

void UMessageWidgetBase::UpdateLargeNotice()
{
	FSimpleNoticeData Data;
	if (LargeNoticeQueue.Dequeue(Data))
	{
		if (Data.IsValidData())
		{
			PlayAnimation(LargeNoticeAnim);
			LargeNoticeText->SetText(Data.Message, true);
			GetWorld()->GetTimerManager().SetTimer(LargeNoticeTimer,
				this, &UMessageWidgetBase::UpdateLargeNotice,
				Data.CalcDisplayTime(), false);
		}
		else
		{
			UpdateLargeNotice();
		}
	}
	else
	{
		PlayAnimationReverse(LargeNoticeAnim);
		LargeNoticeTimer.Invalidate();
	}
}

void UMessageWidgetBase::UpdateSubtitle()
{
	FSimpleSubtitleData Data;
	if (SubtitleQueue.Dequeue(Data))
	{
		if (Data.IsValidData())
		{
			PlayAnimation(SubtitleAnim);
			SubtitleNameText->SetText(Data.Speaker);
			SubtitleLineText->SetText(Data.Line, true);
			GetWorld()->GetTimerManager().SetTimer(SubtitleTimer,
				this, &UMessageWidgetBase::UpdateSubtitle,
				Data.CalcDisplayTime(), false);
		}
		else
		{
			UpdateSubtitle();
		}
	}
	else
	{
		PlayAnimationReverse(SubtitleAnim);
		SubtitleTimer.Invalidate();
	}
}

void UMessageWidgetBase::HideCheck()
{
	HideCheckTime = 0.0f;
	if (!WorldSettings || IsAnimationPlaying(SubtitlePauseFade)) return;
	const bool bNewAutoHidden = IsValid(WorldSettings->GetPauserPlayerState());
	if (bAutoHidden != bNewAutoHidden)
	{
		bAutoHidden = bNewAutoHidden;
		bAutoHidden ? PlayAnimationForward(SubtitlePauseFade) : PlayAnimationReverse(SubtitlePauseFade);
	}
}

void UMessageWidgetBase::InitWidget()
{
	WorldSettings = GetWorld()->GetWorldSettings();
}

void UMessageWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (HideCheckTime >= 0.25f) { HideCheck(); }
	else { HideCheckTime += InDeltaTime; }
}
