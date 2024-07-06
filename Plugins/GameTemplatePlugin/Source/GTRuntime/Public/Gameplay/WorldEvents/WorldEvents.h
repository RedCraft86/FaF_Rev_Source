// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "InstancedStruct.h"
#include "GameplayTagContainer.h"
#include "UObject/SoftObjectPtr.h"
#include "Components/AudioComponent.h"
#include "GameMusic/MusicDataBase.h"
#include "WorldEvents.generated.h"

class AGTActor;
class AAmbientSound;
class AStaticMeshActor;
class ALevelSequenceActor;
class UGTGlobalSubsystem;

UENUM(BlueprintType, DisplayName = "Event Change Type")
enum class EWEChangeType : uint8
{
	Set,
	Toggle
};

/* Prefixes
 * WC - World Control: For flow control types, like a delay.
 * WE - World Event: For regular tasks.
 */
USTRUCT(BlueprintInternalUseOnly)
struct GTRUNTIME_API FWorldEventBase
{
	GENERATED_BODY()
	
	bool bRequiresTick;
	bool bRunConstructionBeforeBeginPlay;

	FWorldEventBase() : bRequiresTick(false), bRunConstructionBeforeBeginPlay(false) {}
	virtual ~FWorldEventBase() = default;
	
	virtual void RunEvent(const UObject* WorldContext) {}
	virtual void OnBeginPlay(const UObject* WorldContext);
	virtual void OnTick(const UObject* WorldContext, const float DeltaTime) {}
	virtual void OnConstruction(const UObject* WorldContext, const bool bEditorTime) {}

protected:

	static inline uint8 LatentID = 255;
	static FLatentActionInfo GetLatentInfo()
	{
		FLatentActionInfo Info;
		Info.Linkage = 0;
		Info.UUID = LatentID--;
		Info.CallbackTarget = nullptr;
		Info.ExecutionFunction = NAME_None;
		return Info;
	}
};

USTRUCT(BlueprintInternalUseOnly, DisplayName = "Misc", meta = (Hidden))
struct GTRUNTIME_API FWEMiscBase : public FWorldEventBase
{ GENERATED_BODY() };

USTRUCT(BlueprintInternalUseOnly, DisplayName = "Control", meta = (Hidden))
struct GTRUNTIME_API FWCBase : public FWorldEventBase
{ GENERATED_BODY() };

USTRUCT(BlueprintType, DisplayName = "Wait, Then Do")
struct GTRUNTIME_API FWCDelay final : public FWCBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Delay", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		float Delay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Delay")
		bool bRetriggerable;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Delay", meta = (BaseStruct = "/Script/GTRuntime.WorldEventBase", ExcludeBaseStruct, ShowTreeView))
		TArray<FInstancedStruct> Events;

	FWCDelay() : Delay(0.25f), bRetriggerable(false) {}

protected:

	FTimerHandle Handle;
	virtual void RunEvent(const UObject* WorldContext) override;
	virtual void OnConstruction(const UObject* WorldContext, const bool bEditorTime) override;
};

USTRUCT(BlueprintType, DisplayName = "Flip Flop")
struct GTRUNTIME_API FWCFlipFlop final : public FWCBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FlipFlop", DisplayName = "Flip", meta = (BaseStruct = "/Script/GTRuntime.WorldEventBase", ExcludeBaseStruct, ShowTreeView))
		TArray<FInstancedStruct> EventsA;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FlipFlop", DisplayName = "Flop", meta = (BaseStruct = "/Script/GTRuntime.WorldEventBase", ExcludeBaseStruct, ShowTreeView))
		TArray<FInstancedStruct> EventsB;

	FWCFlipFlop() : bIsA(false) {}

protected:

	bool bIsA;
	virtual void RunEvent(const UObject* WorldContext) override;
	virtual void OnConstruction(const UObject* WorldContext, const bool bEditorTime) override;
};

USTRUCT(BlueprintType, DisplayName = "Do Once")
struct GTRUNTIME_API FWCDoOnce final : public FWCBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DoOnce", meta = (BaseStruct = "/Script/GTRuntime.WorldEventBase", ExcludeBaseStruct, ShowTreeView))
		TArray<FInstancedStruct> Events;

	FWCDoOnce() : bHasDone(false) {}

protected:

	bool bHasDone;
	virtual void RunEvent(const UObject* WorldContext) override;
	virtual void OnConstruction(const UObject* WorldContext, const bool bEditorTime) override;
};

USTRUCT(BlueprintInternalUseOnly, DisplayName = "Events", meta = (Hidden))
struct GTRUNTIME_API FWEEventsBase : public FWorldEventBase
{ GENERATED_BODY() };

USTRUCT(BlueprintType, DisplayName = "Global Event")
struct GTRUNTIME_API FWEEventGlobal final : public FWEEventsBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GlobalEvent")
		FGameplayTag EventID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GlobalEvent")
		FInstancedStruct Payload;

	FWEEventGlobal() : EventID({}), Payload({}) {}

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Remote Event")
struct GTRUNTIME_API FWEEventRemote final : public FWEEventsBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RemoteEvent")
		FName EventName;

	FWEEventRemote() : EventName(NAME_None) {}

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Actor Event")
struct GTRUNTIME_API FWEEventActor final : public FWEEventsBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ActorEvent")
		FName EventName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ActorEvent")
		TSet<TSoftObjectPtr<AActor>> Targets;

	FWEEventActor() : EventName(NAME_None), Targets({}) {}

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintInternalUseOnly, DisplayName = "Actors", meta = (Hidden))
struct GTRUNTIME_API FWEActorsBase : public FWorldEventBase
{ GENERATED_BODY() };

USTRUCT(BlueprintType, DisplayName = "Actor Visibility")
struct GTRUNTIME_API FWEActorVisibility final : public FWEActorsBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ActorVisibility")
		EWEChangeType ChangeType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ActorVisibility", meta = (EditCondition = "ChangeType == EWEChangeType::Set", EditConditionHides))
		bool bVisible;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ActorVisibility")
		TSet<TSoftObjectPtr<AActor>> Targets;

	FWEActorVisibility() : ChangeType(EWEChangeType::Set), bVisible(true), Targets({}) {}

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Actor Collision")
struct GTRUNTIME_API FWEActorCollision final : public FWEActorsBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ActorCollision")
		EWEChangeType ChangeType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ActorCollision", meta = (EditCondition = "ChangeType == EWEChangeType::Set", EditConditionHides))
		bool bCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ActorCollision")
		TSet<TSoftObjectPtr<AActor>> Targets;

	FWEActorCollision() : ChangeType(EWEChangeType::Set), bCollision(true), Targets({}) {}

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Actor Tags")
struct GTRUNTIME_API FWEActorTags final : public FWEActorsBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ActorTags")
		bool bRemoval;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ActorTags")
		TSet<FName> Tags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ActorTags")
		TSet<TSoftObjectPtr<AActor>> Targets;

	FWEActorTags() : bRemoval(false), Tags({}), Targets({}) {}

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Actor Enabled")
struct GTRUNTIME_API FWEActorEnabled final : public FWEActorsBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ActorEnabled")
		EWEChangeType ChangeType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ActorEnabled", meta = (EditCondition = "ChangeType == EWEChangeType::Set", EditConditionHides))
		bool bEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ActorEnabled", meta = (AllowedClasses = "/Script/GTRuntime.GTActor,/Script/GTRuntime.GTVolume"))
		TSet<TSoftObjectPtr<AActor>> Targets;

	FWEActorEnabled() : ChangeType(EWEChangeType::Set), bEnabled(true), Targets({}) {}

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintInternalUseOnly, DisplayName = "Static Mesh", meta = (Hidden))
struct GTRUNTIME_API FWEStaticMeshBase : public FWorldEventBase
{ GENERATED_BODY() };

USTRUCT(BlueprintType, DisplayName = "Static Mesh Material")
struct GTRUNTIME_API FWEStaticMeshMaterial final : public FWEStaticMeshBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshMaterial")
		TMap<uint8, TSoftObjectPtr<UMaterialInterface>> Materials;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshMaterial")
		TSet<TSoftObjectPtr<AStaticMeshActor>> Targets;

	FWEStaticMeshMaterial() : Materials({}), Targets({}) {}

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Static Mesh Primitive Data")
struct GTRUNTIME_API FWEStaticMeshPrimitiveData final : public FWEStaticMeshBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshPrimitiveData")
		TMap<uint8, float> Data;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshPrimitiveData")
		TSet<TSoftObjectPtr<AStaticMeshActor>> Targets;

	FWEStaticMeshPrimitiveData() : Data({}), Targets({}) {}

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintInternalUseOnly, DisplayName = "Sound", meta = (Hidden))
struct GTRUNTIME_API FWESoundBase : public FWorldEventBase
{ GENERATED_BODY() };

USTRUCT(BlueprintType, DisplayName = "World Sound")
struct GTRUNTIME_API FWESoundWorld final : public FWESoundBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldSound")
		bool bStopping;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldSound", meta = (EditCondition = "!bStopping", EditConditionHides))
		float StartTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldSound")
		bool bFade;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldSound", meta = (ClampMin = 0.1f, UIMin = 0.1f, EditCondition = "bFade", EditConditionHides))
		float FadeTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldSound", meta = (EditCondition = "bFade", EditConditionHides))
		EAudioFaderCurve FadeCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldSound")
		TSet<TSoftObjectPtr<AAmbientSound>> Targets;

	FWESoundWorld()
		: bStopping(false), StartTime(0.0f), bFade(false), FadeTime(1.0f)
		, FadeCurve(EAudioFaderCurve::Linear), Targets({})
	{}

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Play Sound 2D")
struct GTRUNTIME_API FWESoundPlay2D final : public FWESoundBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlaySound2D")
		TObjectPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlaySound2D", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		float Volume;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlaySound2D", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		float Pitch;

	FWESoundPlay2D() : Volume(1.0f), Pitch(1.0f) {}

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "World Music")
struct GTRUNTIME_API FWEWorldMusic final : public FWESoundBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldMusic")
		EWorldMusicChannel Channel;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldMusic")
		TObjectPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldMusic", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		float Volume;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldMusic", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		float Pitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldMusic", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float StartTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldMusic")
		bool bFade;

	FWEWorldMusic() : Channel(EWorldMusicChannel::Channel1), Volume(1.0f), Pitch(1.0f), StartTime(0.0f), bFade(false) {}

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintInternalUseOnly, DisplayName = "Level Stream", meta = (Hidden))
struct GTRUNTIME_API FWELevelStreamBase : public FWorldEventBase
{ GENERATED_BODY() };

USTRUCT(BlueprintType, DisplayName = "Load Stream Level")
struct GTRUNTIME_API FWELevelStreamLoad final : public FWELevelStreamBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RemoteEvent")
		TSoftObjectPtr<UWorld> World;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RemoteEvent")
		bool bMakeVisibleAfterLoad;

	FWELevelStreamLoad() : World(nullptr), bMakeVisibleAfterLoad(true) {}

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Unload Stream Level")
struct GTRUNTIME_API FWELevelStreamUnload final : public FWELevelStreamBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RemoteEvent")
		TSoftObjectPtr<UWorld> World;

	FWELevelStreamUnload() : World(nullptr) {}

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Sequencer")
struct GTRUNTIME_API FWESequencer final : public FWEMiscBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sequencer")
		float PlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sequencer")
		TSet<TSoftObjectPtr<ALevelSequenceActor>> PlayTargets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sequencer")
		TSet<TSoftObjectPtr<ALevelSequenceActor>> StopTargets;

	FWESequencer() : PlayRate(1.0f) {}

protected:
	
	virtual void RunEvent(const UObject* WorldContext) override;
};
