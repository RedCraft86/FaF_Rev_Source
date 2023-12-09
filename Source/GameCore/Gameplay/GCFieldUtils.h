// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "InstancedStruct.h"
#include "RCUtilsSubsystem.h"
#include "Sound/AmbientSound.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "GameFramework/Actor.h"
#include "Misc/OutputDeviceNull.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/LevelScriptActor.h"
#include "Components/AudioComponent.h"
#include "Player/GCPlayerController.h"
#include "UserInterface/Messaging/GCMessageWidget.h"
#include "GCFieldUtils.generated.h"

UENUM(BlueprintType, DisplayName = "Event Change Type")
enum class EGCEventChangeType : uint8
{
	Set,
	Toggle
};

USTRUCT(BlueprintInternalUseOnly)
struct GAMECORE_API FGCEvent
{
	GENERATED_BODY()
	
	virtual ~FGCEvent() = default;
	virtual void OnTick(const float DeltaTime) {}
	virtual void RunEvent(const UObject* WorldContext) {}
};

USTRUCT(BlueprintType, DisplayName = "Wait, Then Do")
struct GAMECORE_API FGCDelayedEvent : public FGCEvent
{
	GENERATED_BODY()

	FGCDelayedEvent() : bRetriggerable(false), Delay(1.0f), Events({}) {}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		bool bRetriggerable;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		float Delay;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (BaseStruct = "/Script/GameCore.GCEvent", ExcludeBaseStruct))
		TArray<FInstancedStruct> Events;

protected:

	FTimerHandle Handle;
	virtual void RunEvent(const UObject* WorldContext) override
	{
		if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull))
		{
			FTimerManager& Timer = World->GetTimerManager();
			if (Timer.IsTimerActive(Handle) && !bRetriggerable) return;
			
			Timer.ClearTimer(Handle);
			Timer.SetTimer(Handle, [this, WorldContext]()
			{
				for (FInstancedStruct& Event : Events)
				{
					if (FGCEvent* EventPtr = Event.GetMutablePtr<FGCEvent>())
					{
						EventPtr->RunEvent(WorldContext);
					}
				}
			}, Delay, false);
		}
	}
};

USTRUCT(BlueprintType, DisplayName = "Run Global Event")
struct GAMECORE_API FGCGlobalEvent : public FGCEvent
{
	GENERATED_BODY()

	FGCGlobalEvent()
		: EventID({})
		, Payload({})
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GlobalEvent")
		FGameplayTag EventID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GlobalEvent")
		FInstancedStruct Payload;

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override
	{
		if (const URCUtilsSubsystem* Subsystem = URCUtilsSubsystem::Get(WorldContext))
		{
			Subsystem->InvokeGlobalEvent(EventID, const_cast<UObject*>(WorldContext), Payload);
		}
	}
};

USTRUCT(BlueprintType, DisplayName = "Run Local Event")
struct GAMECORE_API FGCLocalEvent : public FGCEvent
{
	GENERATED_BODY()

	FGCLocalEvent()
		: EventNames({})
		, Targets({})
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LocalEvent")
		TSet<FName> EventNames;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LocalEvent")
		TSet<AActor*> Targets;

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override
	{
		for (AActor* Target : Targets)
		{
			if (!Target) continue;

			for (const FName& Name : EventNames)
			{
				if (Name.IsNone()) continue;

				FOutputDeviceNull Ar;
				Target->CallFunctionByNameWithArguments(*Name.ToString(), Ar, nullptr, true);
			}
		}
	}
};

USTRUCT(BlueprintType, DisplayName = "Run Remote Event")
struct GAMECORE_API FGCRemoteEvent : public FGCEvent
{
	GENERATED_BODY()

	FGCRemoteEvent()
		: EventNames({})
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LocalEvent")
		TSet<FName> EventNames;

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
		if (ALevelScriptActor* LSA = World ? World->GetLevelScriptActor() : nullptr)
		{
			for (const FName& Name : EventNames)
			{
				LSA->RemoteEvent(Name);
			}
		}
	}
};

USTRUCT(BlueprintType, DisplayName = "Display Notice")
struct GAMECORE_API FGCNoticeEvent : public FGCEvent
{
	GENERATED_BODY()

	FGCNoticeEvent()
		: bObjectOverrideQueue(true)
		, NoticeData({})
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notice")
		bool bObjectOverrideQueue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notice")
		FGCNoticeData NoticeData;
	
protected:
	
	virtual void RunEvent(const UObject* WorldContext) override
	{
		if (const AGCPlayerController* PC = AGCPlayerController::Get(WorldContext))
		{
			PC->GetUserWidget<UGCMessageWidget>()->QueueNoticeByObject(NoticeData, bObjectOverrideQueue ? WorldContext : nullptr);
		}
	}
};

USTRUCT(BlueprintType, DisplayName = "Display Subtitle")
struct GAMECORE_API FGCSubtitleEvent : public FGCEvent
{
	GENERATED_BODY()

	FGCSubtitleEvent()
		: bObjectOverrideQueue(true)
		, SubtitleData({})
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Subtitle")
		bool bObjectOverrideQueue;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Subtitle")
		FGCSubtitleData SubtitleData;

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override
	{
		if (const AGCPlayerController* PC = AGCPlayerController::Get(WorldContext))
		{
			PC->GetUserWidget<UGCMessageWidget>()->QueueSubtitleByObject(SubtitleData, bObjectOverrideQueue ? WorldContext : nullptr);
		}
	}
};

USTRUCT(BlueprintType, DisplayName = "Play Sequence")
struct GAMECORE_API FGCSequencerEvent : public FGCEvent
{
	GENERATED_BODY()

	FGCSequencerEvent()
		: Target(nullptr)
		, PlayRate(1.0f)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sequence")
		ALevelSequenceActor* Target;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sequence")
		float PlayRate;

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override
	{
		if (FMath::IsNearlyZero(PlayRate)) return;
		if (ULevelSequencePlayer* Player = Target ? Target->GetSequencePlayer() : nullptr)
		{
			Player->SetPlayRate(FMath::Abs(PlayRate));
			PlayRate < 0.0f ? Player->PlayReverse() : Player->Play();
		}
	}
};

USTRUCT(BlueprintType, DisplayName = "Change Visibility")
struct GAMECORE_API FGCVisibilityEvent : public FGCEvent
{
	GENERATED_BODY()

	FGCVisibilityEvent()
		: ChangeType(EGCEventChangeType::Set)
		, ToState(true)
		, Targets({})
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visibility")
		EGCEventChangeType ChangeType;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visibility", meta = (EditCondition = "ChangeType == EGCEventChangeType::Set", EditConditionHides))
		bool ToState;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visibility")
		TSet<AActor*> Targets;
	
protected:

	virtual void RunEvent(const UObject* WorldContext) override
	{
		switch (ChangeType)
		{
		case EGCEventChangeType::Set:
			for (AActor* Target : Targets)
			{
				Target->SetActorHiddenInGame(!ToState);
			}
			break;

		case EGCEventChangeType::Toggle:
			for (AActor* Target : Targets)
			{
				Target->SetActorHiddenInGame(!Target->IsHidden());
			}
			break;
		}
	}
};

USTRUCT(BlueprintType, DisplayName = "Change Collision")
struct GAMECORE_API FGCCollisionEvent : public FGCEvent
{
	GENERATED_BODY()

	FGCCollisionEvent()
		: ChangeType(EGCEventChangeType::Set)
		, ToState(true)
		, Targets({})
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision")
		EGCEventChangeType ChangeType;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision", meta = (EditCondition = "ChangeType == EGCEventChangeType::Set", EditConditionHides))
		bool ToState;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision")
		TSet<AActor*> Targets;
	
protected:

	virtual void RunEvent(const UObject* WorldContext) override
	{
		switch (ChangeType)
		{
		case EGCEventChangeType::Set:
			for (AActor* Target : Targets)
			{
				Target->SetActorEnableCollision(ToState);
			}
			break;

		case EGCEventChangeType::Toggle:
			for (AActor* Target : Targets)
			{
				Target->SetActorEnableCollision(!Target->GetActorEnableCollision());
			}
			break;
		}
	}
};

USTRUCT(BlueprintType, DisplayName = "Play Sound Actor")
struct GAMECORE_API FGCPlaySoundEvent : public FGCEvent
{
	GENERATED_BODY()

	FGCPlaySoundEvent()
		: StartTime(0.0f)
		, bFade(false)
		, FadeTime(1.0f)
		, FadeCurve(EAudioFaderCurve::Linear)
		, Target(nullptr)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Play Sound", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float StartTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Play Sound")
		bool bFade;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Play Sound", meta = (ClampMin = 0.1f, UIMin = 0.1f, EditCondition = "bFadeIn", EditConditionHides))
		float FadeTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Play Sound", meta = (EditCondition = "bFadeIn", EditConditionHides))
		EAudioFaderCurve FadeCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Play Sound")
		AAmbientSound* Target;

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override
	{
		if (UAudioComponent* AudioComp = Target ? Target->GetAudioComponent() : nullptr)
		{
			if (AudioComp->IsPlaying()) return;
			if (bFade && FadeTime > 0.0f)
			{
				AudioComp->FadeIn(FadeTime, 0.0f, StartTime, FadeCurve);
			}
			else
			{
				AudioComp->Play(StartTime);
			}
		}
	}
};

USTRUCT(BlueprintType, DisplayName = "Stop Sound Actor")
struct GAMECORE_API FGCSoundPlayEvent : public FGCEvent
{
	GENERATED_BODY()

	FGCSoundPlayEvent()
		: bFade(false)
		, FadeTime(1.0f)
		, FadeCurve(EAudioFaderCurve::Linear)
		, Target(nullptr)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stop Sound")
		bool bFade;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stop Sound", meta = (ClampMin = 0.1f, UIMin = 0.1f, EditCondition = "bFadeIn", EditConditionHides))
		float FadeTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stop Sound", meta = (EditCondition = "bFadeIn", EditConditionHides))
		EAudioFaderCurve FadeCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stop Sound")
		AAmbientSound* Target;

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override
	{
		if (UAudioComponent* AudioComp = Target ? Target->GetAudioComponent() : nullptr)
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
};

USTRUCT(BlueprintType, DisplayName = "Change Material")
struct GAMECORE_API FGCMaterialEvent : public FGCEvent
{
	GENERATED_BODY()

	FGCMaterialEvent()
		: Target(nullptr)
		, ChangeTo({})
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Subtitle")
		AStaticMeshActor* Target;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Subtitle")
		TMap<uint8, UMaterialInterface*> ChangeTo;

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override
	{
		if (!Target) return;
		for (const TPair<uint8, UMaterialInterface*>& Pair : ChangeTo)
		{
			if (!Pair.Value) continue;
			Target->GetStaticMeshComponent()->SetMaterial(Pair.Key, Pair.Value);
		}
	}
};

USTRUCT(BlueprintType, DisplayName = "Change Primitive Data")
struct GAMECORE_API FGCPrimitiveDataEvent : public FGCEvent
{
	GENERATED_BODY()

	FGCPrimitiveDataEvent()
		: Target(nullptr)
		, ChangeTo({})
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Subtitle")
		AStaticMeshActor* Target;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Subtitle")
		TMap<uint8, float> ChangeTo;

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override
	{
		if (!Target) return;
		for (const TPair<uint8, float>& Pair : ChangeTo)
		{
			Target->GetStaticMeshComponent()->SetCustomPrimitiveDataFloat(Pair.Key, Pair.Value);
		}
	}
};
