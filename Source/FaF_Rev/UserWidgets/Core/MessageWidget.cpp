// Copyright (C) RedCraft86. All Rights Reserved.

#include "MessageWidget.h"

void UMessageWidgetBase::QueueSmallNotice(const FSimpleNoticeData& NoticeData, const bool bResetQueue)
{
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
			GetWorld()->GetTimerManager().SetTimer(SmallNoticeTimer,
				this, &UMessageWidgetBase::UpdateSmallNotice,
				Data.CalcDisplayTime());
		}

		UpdateSmallNotice();
	}

	SmallNoticeTimer.Invalidate();
}

void UMessageWidgetBase::UpdateLargeNotice()
{
}

void UMessageWidgetBase::UpdateSubtitle()
{
}
