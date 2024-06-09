// Copyright (C) RedCraft86. All Rights Reserved.

#include "WorldEvents/WorldEventActor.h"

AWorldEventActor::AWorldEventActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
#if WITH_EDITORONLY_DATA
	SceneRoot->bVisualizeComponent = true;
#endif

	WorldEventComponent = CreateDefaultSubobject<UWorldEventComponent>("WorldEvents");
}

void AWorldEventActor::RunEvents() const
{
	WorldEventComponent->RunEvents();
}
