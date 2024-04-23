// Copyright (C) RedCraft86. All Rights Reserved.

#include "MessageWidget.h"
#include "ExprTextBlock.h"
#include "Components/TextBlock.h"

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

void UMessageWidgetBase::QueueSubtitles(const TArray<FSimpleSubtitleData>& Subtitles)
{
	for (const FSimpleSubtitleData& Subtitle : Subtitles)
	{
		QueueSubtitle(Subtitle);
	}
}

void UMessageWidgetBase::QueueSubtitle(const FSimpleSubtitleData& SubtitleData)
{
	if (!SubtitleData.IsValidData()) return;
	
	SubtitleQueue.Enqueue(SubtitleData);
	if (!SubtitleTimer.IsValid() || !GetWorld()->GetTimerManager().IsTimerActive(SubtitleTimer))
	{
		UpdateSubtitle();
	}
}

void UMessageWidgetBase::SetSubtitlesHidden(const bool bHidden)
{
	if (bHidden)
	{
		PlayAnimationForward(SubtitleHideAnim);
	}
	else
	{
		PlayAnimationReverse(SubtitleHideAnim);
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
			return;
		}

		UpdateSmallNotice();
		return;
	}

	PlayAnimationReverse(SmallNoticeAnim);
	SmallNoticeTimer.Invalidate();
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
			return;
		}

		UpdateLargeNotice();
		return;
	}

	PlayAnimationReverse(LargeNoticeAnim);
	LargeNoticeTimer.Invalidate();
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
			return;
		}

		UpdateSubtitle();
		return;
	}

	PlayAnimationReverse(SubtitleAnim);
	SubtitleTimer.Invalidate();
}
