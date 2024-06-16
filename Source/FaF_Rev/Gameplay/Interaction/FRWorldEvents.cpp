// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRWorldEvents.h"
#include "NarrativeComponent.h"
#include "SaveSystem/SaveSubsystem.h"
#include "GameSection/GameSectionManager.h"
#include "FRGameMode.h"

void FWEPlayerSettings::RunEvent(const UObject* WorldContext)
{
	if (!Player.LoadSynchronous())
		Player = AFRPlayerBase::Get(WorldContext);

	if (AFRPlayerBase* PlayerPtr = Player.LoadSynchronous())
	{
		PlayerPtr->SetPlayerSettings(Settings);
	}
}

void FWEPlayerLockFlag::RunEvent(const UObject* WorldContext)
{
	if (!Player.LoadSynchronous())
		Player = AFRPlayerBase::Get(WorldContext);

	if (AFRPlayerBase* PlayerPtr = Player.LoadSynchronous())
	{
		bClearFlag ? PlayerPtr->ClearLockFlag(LockFlag.SelectedValue) : PlayerPtr->AddLockFlag(LockFlag.SelectedValue);
	}
}

void FWEPlayerFade::RunEvent(const UObject* WorldContext)
{
	if (!Player.LoadSynchronous())
		Player = AFRPlayerBase::Get(WorldContext);

	if (const AFRPlayerBase* PlayerPtr = Player.LoadSynchronous())
	{
		bFadeOut ? PlayerPtr->FadeToBlack(FadeTime, bFadeAudio) : PlayerPtr->FadeFromBlack(FadeTime, bFadeAudio);
	}
}

void FWEPlayerLockOn::RunEvent(const UObject* WorldContext)
{
	if (!Player.LoadSynchronous())
		Player = AFRPlayerBase::Get(WorldContext);

	Player->LockOnSpeed = LockOnSpeed;
	if (const AActor* Actor = Target.LoadSynchronous())
	{
		Player->LockOnTarget = Actor->GetRootComponent();
	}
	else
	{
		Player->LockOnTarget = nullptr;
	}
}

void FWEUnlockTransientKey::RunEvent(const UObject* WorldContext)
{
	if (const USaveSubsystem* Subsystem = USaveSubsystem::Get(WorldContext))
	{
		Subsystem->UnlockTransientKey(TransientKey);
	}
}

void FWEUnlockContentKey::RunEvent(const UObject* WorldContext)
{
	if (const USaveSubsystem* Subsystem = USaveSubsystem::Get(WorldContext))
	{
		Subsystem->UnlockContent(ContentKey);
	}
}

void FWESimpleNoticeSmall::RunEvent(const UObject* WorldContext)
{
	if (AFRGameModeBase* GM = FRGameMode(WorldContext))
	{
		for (const FSimpleNoticeData& Notice : Notices)
			GM->QueueSmallNotice(Notice, bResetQueue);
	}
}

void FWESimpleNoticeLarge::RunEvent(const UObject* WorldContext)
{
	if (AFRGameModeBase* GM = FRGameMode(WorldContext))
	{
		for (const FSimpleNoticeData& Notice : Notices)
			GM->QueueLargeNotice(Notice, bResetQueue);
	}
}

void FWESimpleSubtitle::RunEvent(const UObject* WorldContext)
{
	if (AFRGameModeBase* GM = FRGameMode(WorldContext))
	{
		GM->QueueSubtitles(Subtitles);
	}
}

void FWESimpleGuide::RunEvent(const UObject* WorldContext)
{
	if (AFRGameModeBase* GM = FRGameMode(WorldContext))
	{
		GM->QueueGuidePages(Pages);
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

void FWEStepSequence::RunEvent(const UObject* WorldContext)
{
	if (!Player.LoadSynchronous())
		Player = AFRPlayerBase::Get(WorldContext);

	if (AFRPlayerBase* PlayerPtr = Player.LoadSynchronous())
	{
		if (bLock) PlayerPtr->AddLockFlag(Player::LockFlags::Loading);
		if (FMath::Abs(FadeTime) > 0.01f) PlayerPtr->FadeToBlack(FMath::Abs(FadeTime));
	}

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull))
	{
		FTimerManager& Timer = World->GetTimerManager();

		FTimerHandle Handle;
		Timer.SetTimer(Handle, [this, WorldContext]()
		{
			if (UGameSectionManager* Manager = UGameSectionManager::Get(WorldContext))
			{
				Manager->Step(Index);
			}
		}, FMath::Abs(FadeTime), false);
	}
}
