// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRWorldEvents.h"
#include "NarrativeComponent.h"
#include "FRGameMode.h"

void FWEGameNoticeSmall::RunEvent(const UObject* WorldContext)
{
	if (AFRGameModeBase* GM = FRGameMode(WorldContext))
	{
		for (const FSimpleNoticeData& Notice : Notices)
			GM->QueueSmallNotice(Notice, bResetQueue);
	}
}

void FWEGameNoticeLarge::RunEvent(const UObject* WorldContext)
{
	if (AFRGameModeBase* GM = FRGameMode(WorldContext))
	{
		for (const FSimpleNoticeData& Notice : Notices)
			GM->QueueLargeNotice(Notice, bResetQueue);
	}
}

void FWEGameSubtitle::RunEvent(const UObject* WorldContext)
{
	if (AFRGameModeBase* GM = FRGameMode(WorldContext))
	{
		GM->QueueSubtitles(Subtitles);
	}
}

void FWENarrativeDialogue::RunEvent(const UObject* WorldContext)
{
	if (const AFRGameModeBase* GM = FRGameMode(WorldContext))
	{
		if (!GM->Narrative->IsInDialogue())
			GM->Narrative->BeginDialogue(Dialogue.LoadSynchronous(), NAME_None);
	}
}

void FWENarrativeQuest::RunEvent(const UObject* WorldContext)
{
	if (const AFRGameModeBase* GM = FRGameMode(WorldContext))
	{
		if (!GM->Narrative->IsQuestInProgress(Quest.LoadSynchronous()))
			GM->Narrative->BeginQuest(Quest.LoadSynchronous(), NAME_None);
	}
}

void FWENarrativeTask::RunEvent(const UObject* WorldContext)
{
	if (const AFRGameModeBase* GM = FRGameMode(WorldContext))
	{
		if (!Task.IsNull() && !Argument.IsEmpty() && Quantity > 0)
			GM->Narrative->CompleteNarrativeDataTask(Task.LoadSynchronous(), Argument, Quantity);
	}
}
