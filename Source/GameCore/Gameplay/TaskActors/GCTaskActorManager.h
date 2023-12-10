// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "GameSequence/GCSequenceTypes.h"
#include "GCTaskActorManager.generated.h"

UCLASS(NotBlueprintable, NotBlueprintType)
class GAMECORE_API UGCTaskActorManagerComponent final : public UActorComponent
{
	GENERATED_BODY()

public:

	UGCTaskActorManagerComponent()
	{
		PrimaryComponentTick.bCanEverTick = false;
		PrimaryComponentTick.bStartWithTickEnabled = false;
#if WITH_EDITORONLY_DATA
		SetIsVisualizationComponent(true);
#endif
	}
#if WITH_EDITORONLY_DATA
	TMap<FVector, FLinearColor> Targets;
#endif
private:
	virtual void BeginPlay() override
	{
		Super::BeginPlay();
		DestroyComponent();
	}
};

UCLASS(NotBlueprintable, DisplayName = "Task Actor Manager")
class GAMECORE_API AGCTaskActorManager final : public AActor
{
	GENERATED_BODY()

public:
	
	AGCTaskActorManager();

	UPROPERTY(VisibleDefaultsOnly, Category = "DefaultObjects")
		USceneComponent* SceneRoot;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaskActorManager")
		FGCSequenceID Sequence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaskActorManager")
		uint8 SubsequenceIndex;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaskActorManager")
		TSet<class AGCTaskActorBase*> TaskActors;

private:
#if WITH_EDITORONLY_DATA
	UGCTaskActorManagerComponent* Visualizer;
#endif
#if WITH_EDITOR
	UBillboardComponent* VisualIcon;
	virtual void OnConstruction(const FTransform& Transform) override;
#endif
	
	virtual void BeginPlay() override;
	void OnSequenceLoaded(const FName& ID);
};
