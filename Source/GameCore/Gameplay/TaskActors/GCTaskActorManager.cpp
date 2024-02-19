// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCTaskActorManager.h"
#include "GCTaskActorBase.h"
#include "GameSequence/GCSequenceManager.h"
#if WITH_EDITOR
#include "UObject/ConstructorHelpers.h"
#include "Components/BillboardComponent.h"
#endif

AGCTaskActorManager::AGCTaskActorManager()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

#if WITH_EDITORONLY_DATA
	bIsSpatiallyLoaded = false;
	Visualizer = CreateEditorOnlyDefaultSubobject<UGCTaskActorManagerComponent>("Visualizer");
#endif
#if WITH_EDITOR
	VisualIcon = CreateEditorOnlyDefaultSubobject<UBillboardComponent>("VisualIcon");
	if (VisualIcon)
	{
		VisualIcon->SetRelativeScale3D_Direct(FVector(0.5f, 0.5f, 0.5f));
		VisualIcon->bIsScreenSizeScaled = true;
		VisualIcon->SetupAttachment(GetRootComponent());
		const ConstructorHelpers::FObjectFinder<UTexture2D> IconFinder(
			TEXT("/Engine/EditorResources/S_ForceFeedbackComponent.S_ForceFeedbackComponent"));
		if (IconFinder.Succeeded())
		{
			VisualIcon->Sprite = IconFinder.Object;
		}
	}
#endif
}

#if WITH_EDITORONLY_DATA
void AGCTaskActorManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (Visualizer)
	{
		Visualizer->Targets.Empty();
		for (const AGCTaskActorBase* Actor : TaskActors)
		{
			if (Actor)
			{
				Visualizer->Targets.Add(Actor->GetActorLocation(), FLinearColor::MakeRandomColor());
			}
		}
	}
}
#endif

void AGCTaskActorManager::BeginPlay()
{
	Super::BeginPlay();

	if (!Sequence.IsValid())
	{
		return;
	}

	if (UGCSequenceManager* Manager = GetWorld()->GetSubsystem<UGCSequenceManager>())
	{
		Manager->OnSequenceChangeFinish.AddUObject(this, &AGCTaskActorManager::OnSequenceLoaded);
		OnSequenceLoaded(*Manager->GetCurrentSequence());
	}

	for (AGCTaskActorBase* Actor : TaskActors)
	{
		if (Actor) Actor->OnTaskCompleted.AddUObject(this, &AGCTaskActorManager::OnTaskComplete);
	}
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void AGCTaskActorManager::OnSequenceLoaded(const FName& ID)
{
	if (const UGCSequenceManager* Manager = GetWorld()->GetSubsystem<UGCSequenceManager>())
	{
		if (ID.ToString() == Sequence.ToString() && Manager->GetSubsequenceIndex() > SubsequenceIndex)
		{
			for (AGCTaskActorBase* Actor : TaskActors)
			{
				if (Actor && !Actor->IsCompleted()) Actor->CompleteTask();
			}
		}
	}
}

void AGCTaskActorManager::OnTaskComplete(AGCTaskActorBase* TaskActor)
{
	uint8 Completed = 0, Total = 0;
	for (const AGCTaskActorBase* Actor : TaskActors)
	{
		if (Actor)
		{
			Total++;
			if (Actor->IsCompleted()) Completed++;
		}
	}

	OnProgressMade.Broadcast(Completed, Total);
}
