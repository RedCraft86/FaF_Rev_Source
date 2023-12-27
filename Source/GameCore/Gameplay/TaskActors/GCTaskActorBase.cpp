// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCTaskActorBase.h"
#include "Components/BillboardComponent.h"
#include "Narrative/GCNarrativeComponent.h"

AGCTaskActorBase::AGCTaskActorBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

#if WITH_EDITORONLY_DATA
	SceneRoot->bVisualizeComponent = true;
	bIsSpatiallyLoaded = false;
#endif

	IconBillboard = CreateDefaultSubobject<UBillboardComponent>("IconBillboard");
	IconBillboard->SetHiddenInGame(false);
	IconBillboard->SetVisibility(false);
	IconBillboard->SetupAttachment(SceneRoot);

	NarrativeTask = nullptr;
	TaskArgument = TEXT("");
	TaskAmount = 1;

	bSingleUse = true;
	IconImage = nullptr;
	IconLocation = FVector::ZeroVector;
	IconScale = 1.0f;
	
	bCompleted = false;
	bTickEnabled = false;
}

void AGCTaskActorBase::LockTask()
{
	bTickEnabled = IsActorTickEnabled();
		
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}

void AGCTaskActorBase::UnlockTask()
{
	if (!(bCompleted && bSingleUse))
	{
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
		SetActorTickEnabled(bTickEnabled);
	}
}

void AGCTaskActorBase::ResetTask()
{
	UnlockTask();
	bCompleted = false;
}

void AGCTaskActorBase::CompleteTask()
{
	if (bCompleted && bSingleUse)
		return;

	bCompleted = true;
	if (bSingleUse)
	{
		LockTask();
	}

	if (NarrativeTask && !TaskArgument.IsEmpty() && TaskAmount != 0)
	{
		if (UGCNarrativeComponent* Narrative = UGCNarrativeComponent::Get(this))
		{
			Narrative->CompleteNarrativeDataTask(NarrativeTask, TaskArgument, TaskAmount);
		}
	}

	OnCompletedEvent();
	OnTaskCompleted.Broadcast(this);
	OnTaskCompletedBP.Broadcast(this);
}

void AGCTaskActorBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	IconBillboard->SetRelativeLocation(IconLocation);
	IconBillboard->SetRelativeScale3D_Direct(FVector(IconScale));
	IconBillboard->SetVisibility(IsValid(IconImage));
	IconBillboard->SetSprite(IconImage);
}

void AGCTaskActorBase::BeginPlay()
{
	Super::BeginPlay();

	if (bStartLocked)
		LockTask();
}
