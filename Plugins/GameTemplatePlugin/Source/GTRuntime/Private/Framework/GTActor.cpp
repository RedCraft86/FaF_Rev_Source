// Copyright (C) RedCraft86. All Rights Reserved.

#include "GTActor.h"

AGTActor::AGTActor() : bEnabled(true), bStartWithCollisionEnabled(true)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
	
#if WITH_EDITORONLY_DATA
	RuntimeGuid = ActorGuid;
#else
	RuntimeGuid = FGuid::NewGuid();
#endif
}

void AGTActor::SetEnabled(const bool bInEnabled)
{
	if (bEnabled != bInEnabled)
	{
		bEnabled = bInEnabled;
		ON_ENABLE_STATE_CHANGED
	}
}

void AGTActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorEnableCollision(bStartWithCollisionEnabled);
	if (!bEnabled) OnEnableStateChanged(bEnabled);
}

void AGTActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (!RuntimeGuid.IsValid())
	{
#if WITH_EDITORONLY_DATA
		RuntimeGuid = ActorGuid;
#else
		RuntimeGuid = FGuid::NewGuid();
#endif
	}
}

void AGTActor::OnEnableStateChanged(const bool bIsEnabled)
{
	SetActorHiddenInGame(!bIsEnabled);
	SetActorEnableCollision(bIsEnabled);
	SetActorTickEnabled(PrimaryActorTick.bStartWithTickEnabled && bIsEnabled);
}
