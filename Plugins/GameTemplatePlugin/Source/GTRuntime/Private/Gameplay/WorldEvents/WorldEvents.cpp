// Copyright (C) RedCraft86. All Rights Reserved.

#include "WorldEvents/WorldEvents.h"
#include "Engine/LevelScriptActor.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/OutputDeviceNull.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "Sound/AmbientSound.h"
#include "GTGlobalSubsystem.h"
#include "GTActor.h"

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
		for (const TSoftObjectPtr<AGTActor>& Actor : Targets)
		{
			if (AGTActor* ActorPtr = Actor.LoadSynchronous())
			{
				ActorPtr->SetEnabled(bEnabled);
			}
		}
		break;

	case EWEChangeType::Toggle:
		for (const TSoftObjectPtr<AGTActor>& Actor : Targets)
		{
			if (AGTActor* ActorPtr = Actor.LoadSynchronous())
			{
				ActorPtr->SetEnabled(!ActorPtr->IsEnabled());
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
				MeshComp->SetCustomPrimitiveDataFloat(Pair.Key, Pair.Value);
			}
		}
	}
}

void FWESoundPlay::RunEvent(const UObject* WorldContext)
{
	for (const TSoftObjectPtr<AAmbientSound>& Actor : Targets)
	{
		const AAmbientSound* ActorPtr = Actor.LoadSynchronous();
		if (UAudioComponent* AudioComp = ActorPtr ? ActorPtr->GetAudioComponent() : nullptr)
		{
			if (AudioComp->IsPlaying()) return;
			if (bFade && FadeTime > 0.0f)
			{
				AudioComp->FadeIn(FadeTime, 0.0f, FMath::Max(0.0f, StartTime), FadeCurve);
			}
			else
			{
				AudioComp->Play();
			}
		}
	}
}

void FWESoundStop::RunEvent(const UObject* WorldContext)
{
	for (const TSoftObjectPtr<AAmbientSound>& Actor : Targets)
	{
		const AAmbientSound* ActorPtr = Actor.LoadSynchronous();
		if (UAudioComponent* AudioComp = ActorPtr ? ActorPtr->GetAudioComponent() : nullptr)
		{
			if (!AudioComp->IsPlaying()) return;
			if (bFade && FadeTime > 0.0f)
			{
				AudioComp->FadeOut(FadeTime, 0.0f, FadeCurve);
			}
			else
			{
				AudioComp->Stop();
			}
		}
	}
}

void FWESequencePlay::RunEvent(const UObject* WorldContext)
{
	if (FMath::IsNearlyZero(PlayRate)) return;
	if (ULevelSequencePlayer* Player = Target.IsNull() ? nullptr : Target.LoadSynchronous()->GetSequencePlayer())
	{
		Player->SetPlayRate(FMath::Abs(PlayRate));
		PlayRate < 0.0f ? Player->PlayReverse() : Player->Play();
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
