// Copyright (C) RedCraft86. All Rights Reserved.

#include "WorldEvents/WorldEvents.h"
#include "Engine/LevelScriptActor.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/OutputDeviceNull.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "Sound/AmbientSound.h"
#include "GameMusic/MusicManagerBase.h"
#include "GTGlobalSubsystem.h"
#include "GTVolume.h"
#include "GTActor.h"

void FWorldEventBase::OnBeginPlay(const UObject* WorldContext)
{
	if (bRunConstructionBeforeBeginPlay)
	{
		OnConstruction(WorldContext, false);
	}
}

void FWCDelay::RunEvent(const UObject* WorldContext)
{
	if (Delay <= 0 || FMath::IsNearlyZero(Delay)) return;
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull))
	{
		FTimerManager& Timer = World->GetTimerManager();
		if (Timer.IsTimerActive(Handle) && !bRetriggerable) return;
			
		Timer.ClearTimer(Handle);
		Timer.SetTimer(Handle, [this, WorldContext]()
		{
			for (FInstancedStruct& Event : Events)
			{
				if (FWorldEventBase* EventPtr = Event.GetMutablePtr<FWorldEventBase>())
				{
					EventPtr->RunEvent(WorldContext);
				}
			}
		}, Delay, false);
	}
}

void FWCDelay::OnConstruction(const UObject* WorldContext, const bool bEditorTime)
{
	for (FInstancedStruct& Event : Events)
	{
		if (FWorldEventBase* EventPtr = Event.GetMutablePtr<FWorldEventBase>())
		{
			EventPtr->OnConstruction(WorldContext, bEditorTime);
		}
	}
}

void FWCFlipFlop::RunEvent(const UObject* WorldContext)
{
	bIsA = !bIsA;
	if (bIsA)
	{
		for (FInstancedStruct& Event : EventsA)
		{
			if (FWorldEventBase* EventPtr = Event.GetMutablePtr<FWorldEventBase>())
			{
				EventPtr->RunEvent(WorldContext);
			}
		}
	}
	else
	{
		for (FInstancedStruct& Event : EventsB)
		{
			if (FWorldEventBase* EventPtr = Event.GetMutablePtr<FWorldEventBase>())
			{
				EventPtr->RunEvent(WorldContext);
			}
		}
	}
}

void FWCFlipFlop::OnConstruction(const UObject* WorldContext, const bool bEditorTime)
{
	TArray<FInstancedStruct> Events = EventsA;
	Events.Append(EventsB);
	
	for (FInstancedStruct& Event : Events)
	{
		if (FWorldEventBase* EventPtr = Event.GetMutablePtr<FWorldEventBase>())
		{
			EventPtr->OnConstruction(WorldContext, bEditorTime);
		}
	}
}

void FWCDoOnce::RunEvent(const UObject* WorldContext)
{
	if (bHasDone) return;
	
	bHasDone = true;
	for (FInstancedStruct& Event : Events)
	{
		if (FWorldEventBase* EventPtr = Event.GetMutablePtr<FWorldEventBase>())
		{
			EventPtr->RunEvent(WorldContext);
		}
	}
}

void FWCDoOnce::OnConstruction(const UObject* WorldContext, const bool bEditorTime)
{
	for (FInstancedStruct& Event : Events)
	{
		if (FWorldEventBase* EventPtr = Event.GetMutablePtr<FWorldEventBase>())
		{
			EventPtr->OnConstruction(WorldContext, bEditorTime);
		}
	}
}

void FWEEventGlobal::RunEvent(const UObject* WorldContext)
{
	if (!EventID.IsValid()) return;
	if (UGTGlobalSubsystem* Subsystem = UGTGlobalSubsystem::Get(WorldContext))
	{
		Subsystem->InvokeGlobalEvent(EventID, const_cast<UObject*>(WorldContext), Payload);
	}
}

void FWEEventRemote::RunEvent(const UObject* WorldContext)
{
	if (EventName.IsNone()) return;
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (ALevelScriptActor* LSA = World ? World->GetLevelScriptActor() : nullptr)
	{
		LSA->RemoteEvent(EventName);
	}
}

void FWEEventActor::RunEvent(const UObject* WorldContext)
{
	if (EventName.IsNone()) return;
	for (const TSoftObjectPtr<AActor>& Actor : Targets)
	{
		if (AActor* ActorPtr = Actor.LoadSynchronous())
		{
			FOutputDeviceNull Ar;
			ActorPtr->CallFunctionByNameWithArguments(*EventName.ToString(),
				Ar, nullptr, true);
		}
	}
}

void FWEActorVisibility::RunEvent(const UObject* WorldContext)
{
	switch (ChangeType)
	{
	case EWEChangeType::Set:
		for (const TSoftObjectPtr<AActor>& Actor : Targets)
		{
			if (AActor* ActorPtr = Actor.LoadSynchronous())
			{
				ActorPtr->SetActorHiddenInGame(!bVisible);
			}
		}
		break;

	case EWEChangeType::Toggle:
		for (const TSoftObjectPtr<AActor>& Actor : Targets)
		{
			if (AActor* ActorPtr = Actor.LoadSynchronous())
			{
				ActorPtr->SetActorHiddenInGame(!ActorPtr->IsHidden());
			}
		}
		break;
	}
}

void FWEActorCollision::RunEvent(const UObject* WorldContext)
{
	switch (ChangeType)
	{
	case EWEChangeType::Set:
		for (const TSoftObjectPtr<AActor>& Actor : Targets)
		{
			if (AActor* ActorPtr = Actor.LoadSynchronous())
			{
				ActorPtr->SetActorEnableCollision(bCollision);
			}
		}
		break;

	case EWEChangeType::Toggle:
		for (const TSoftObjectPtr<AActor>& Actor : Targets)
		{
			if (AActor* ActorPtr = Actor.LoadSynchronous())
			{
				ActorPtr->SetActorEnableCollision(!ActorPtr->GetActorEnableCollision());
			}
		}
		break;
	}
}

void FWEActorTags::RunEvent(const UObject* WorldContext)
{
	for (const TSoftObjectPtr<AActor>& Actor : Targets)
	{
		if (AActor* ActorPtr = Actor.LoadSynchronous())
		{
			for (const FName& Tag : Tags)
			{
				if (Tag.IsNone()) continue;
				if (bRemoval)
				{
					ActorPtr->Tags.Remove(Tag);
				}
				else
				{
					ActorPtr->Tags.AddUnique(Tag);
				}
			}
		}
	}
}

void FWEActorEnabled::RunEvent(const UObject* WorldContext)
{
	switch (ChangeType)
	{
	case EWEChangeType::Set:
		for (const TSoftObjectPtr<AActor>& Actor : Targets)
		{
			if (AActor* ActorPtr = Actor.LoadSynchronous())
			{
				if (AGTActor* GTActor = Cast<AGTActor>(ActorPtr))
				{
					GTActor->SetEnabled(bEnabled);
				}
				else if (AGTVolume* GTVolume = Cast<AGTVolume>(ActorPtr))
				{
					GTVolume->SetEnabled(bEnabled);
				}
			}
		}
		break;

	case EWEChangeType::Toggle:
		for (const TSoftObjectPtr<AActor>& Actor : Targets)
		{
			if (AActor* ActorPtr = Actor.LoadSynchronous())
			{
				if (AGTActor* GTActor = Cast<AGTActor>(ActorPtr))
				{
					GTActor->SetEnabled(!GTActor->IsEnabled());
				}
				else if (AGTVolume* GTVolume = Cast<AGTVolume>(ActorPtr))
				{
					GTVolume->SetEnabled(!GTVolume->IsEnabled());
				}
			}
		}
		break;
	}
}

void FWEStaticMeshMaterial::RunEvent(const UObject* WorldContext)
{
	for (const TSoftObjectPtr<AStaticMeshActor>& Actor : Targets)
	{
		const AStaticMeshActor* ActorPtr = Actor.LoadSynchronous();
		if (UStaticMeshComponent* MeshComp = ActorPtr ? ActorPtr->GetStaticMeshComponent() : nullptr)
		{
			for (const TPair<uint8, TSoftObjectPtr<UMaterialInterface>>& Pair : Materials)
			{
				if (UMaterialInterface* Material = Pair.Value.LoadSynchronous())
				{
					MeshComp->SetMaterial(Pair.Key, Material);
				}
			}
		}
	}
}

void FWEStaticMeshPrimitiveData::RunEvent(const UObject* WorldContext)
{
	for (const TSoftObjectPtr<AStaticMeshActor>& Actor : Targets)
	{
		const AStaticMeshActor* ActorPtr = Actor.LoadSynchronous();
		if (UStaticMeshComponent* MeshComp = ActorPtr ? ActorPtr->GetStaticMeshComponent() : nullptr)
		{
			for (const TPair<uint8, float>& Pair : Data)
			{
				MeshComp->SetDefaultCustomPrimitiveDataFloat(Pair.Key, Pair.Value);
			}
		}
	}
}

void FWESoundWorld::RunEvent(const UObject* WorldContext)
{
	for (const TSoftObjectPtr<AAmbientSound>& Actor : Targets)
	{
		const AAmbientSound* ActorPtr = Actor.LoadSynchronous();
		if (UAudioComponent* AudioComp = ActorPtr ? ActorPtr->GetAudioComponent() : nullptr)
		{
			if (bFade && FadeTime > 0.0f)
			{
				bStopping ? AudioComp->FadeOut(FadeTime, 0.0f, FadeCurve) :
					AudioComp->FadeIn(FadeTime, 1.0f, FMath::Max(0.0f, StartTime), FadeCurve);
			}
			else
			{
				bStopping ? AudioComp->Stop() : AudioComp->Play();
			}
		}
	}
}

void FWESoundPlay2D::RunEvent(const UObject* WorldContext)
{
	UGameplayStatics::PlaySound2D(WorldContext, Sound, FMath::Max(0.0f, Volume), FMath::Max(0.0f, Pitch));	
}

void FWEWorldMusic::RunEvent(const UObject* WorldContext)
{
	if (AMusicManagerBase* MusicManager = AMusicManagerBase::Get(WorldContext))
	{
		MusicManager->AddExternalAudio(Channel, Sound, Volume, Pitch, StartTime, bFade);
	}
}

void FWELevelStreamLoad::RunEvent(const UObject* WorldContext)
{
	UGameplayStatics::LoadStreamLevelBySoftObjectPtr(WorldContext, World,
		bMakeVisibleAfterLoad, false, GetLatentInfo());
}

void FWELevelStreamUnload::RunEvent(const UObject* WorldContext)
{
	UGameplayStatics::UnloadStreamLevelBySoftObjectPtr(WorldContext, World,
		GetLatentInfo(), false);
}

void FWESequencer::RunEvent(const UObject* WorldContext)
{
	for (TSoftObjectPtr<ALevelSequenceActor> StopActor : StopTargets)
	{
		if (StopActor.LoadSynchronous()) StopActor.LoadSynchronous()->GetSequencePlayer()->Stop();
	}

	if (FMath::IsNearlyZero(PlayRate)) return;
	for (TSoftObjectPtr<ALevelSequenceActor> PlayActor : PlayTargets)
	{
		if (!PlayActor.LoadSynchronous()) continue;
		if (ULevelSequencePlayer* Player = PlayActor.LoadSynchronous()->GetSequencePlayer())
		{
			Player->StopAtCurrentTime();
			Player->SetPlayRate(FMath::Abs(PlayRate));
			if (PlayRate < 0.0f)
			{
				Player->GoToEndAndStop();
				Player->PlayReverse();
			}
			else
			{
				Player->SetPlaybackPosition({Player->GetStartTime().Time, EUpdatePositionMethod::Jump});
				Player->Play();
			}
		}
	}
}
