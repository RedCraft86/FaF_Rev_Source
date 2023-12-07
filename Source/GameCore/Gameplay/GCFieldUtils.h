// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "InstancedStruct.h"
#include "RCUtilsSubsystem.h"
#include "Sound/AmbientSound.h"
#include "GameFramework/Actor.h"
#include "Misc/OutputDeviceNull.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/LevelScriptActor.h"
#include "Components/AudioComponent.h"
#include "Player/GCPlayerController.h"
#include "UserInterface/Messaging/GCMessageWidget.h"
#include "GCFieldUtils.generated.h"

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
			if (!bRetriggerable) Timer.ClearTimer(Handle);
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

USTRUCT(BlueprintType, DisplayName = "Toggle Visibility")
struct GAMECORE_API FGCVisibilityEvent : public FGCEvent
{
	GENERATED_BODY()

	FGCVisibilityEvent()
		: bSyncedStates(true)
		, bStartingState(true)
		, Targets({})
		, bCurrentState(true)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visibility")
		bool bSyncedStates;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visibility", meta = (EditCondition = "bSyncedStates", EditConditionHides))
		bool bStartingState;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visibility")
		TSet<AActor*> Targets;
	
protected:

	bool bCurrentState;
	
	virtual void RunEvent(const UObject* WorldContext) override
	{
		if (bSyncedStates)
		{
			bCurrentState = !bStartingState;
			for (AActor* Target : Targets)
			{
				Target->SetActorHiddenInGame(!bCurrentState);
			}
		}
		else
		{
			for (AActor* Target : Targets)
			{
				Target->SetActorHiddenInGame(!Target->IsHidden());
			}
		}
	}
};

USTRUCT(BlueprintType, DisplayName = "Play Sound")
struct GAMECORE_API FGCSoundEvent : public FGCEvent
{
	GENERATED_BODY()

	FGCSoundEvent()
		: Type(TEXT("Local")), SoundWave(nullptr), SoundActor(nullptr)
		, VolumeMultiplier(1.0f), PitchMultiplier(1.0f), StartTime(0.0f)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (GetOptions = "SoundTypes"))
		FName Type;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (EditCondition = "Type == 'Local'", EditConditionHides))
		TSoftObjectPtr<USoundWave> SoundWave;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (EditCondition = "Type == 'Actor'", EditConditionHides))
		AAmbientSound* SoundActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (EditCondition = "Type != 'None'", EditConditionHides))
		float VolumeMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (EditCondition = "Type != 'None'", EditConditionHides))
		float PitchMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (EditCondition = "Type != 'None'", EditConditionHides))
		float StartTime;

protected:

	UPROPERTY(meta = (TransientToolProperty))
		TArray<FName> SoundTypes = {TEXT("Local"), TEXT("Actor")};

	virtual void RunEvent(const UObject* WorldContext) override
	{
		switch (SoundTypes.Find(Type))
		{
		case 0:
			if (!SoundWave.IsNull())
			{
				UGameplayStatics::PlaySound2D(WorldContext, SoundWave.LoadSynchronous(),
					VolumeMultiplier, PitchMultiplier, StartTime);
			}
			break;
			
		case 1:
			if (IsValid(SoundActor))
			{
				if (UAudioComponent* Comp = SoundActor->GetAudioComponent())
				{
					Comp->VolumeMultiplier = VolumeMultiplier;
					Comp->PitchMultiplier = PitchMultiplier;
					Comp->Play(0.0f);
				}
			}
			break;
			
		default: break;
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
