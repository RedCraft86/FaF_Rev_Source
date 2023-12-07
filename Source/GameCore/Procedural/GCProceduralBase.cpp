// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "GCProceduralBase.h"

AGCProceduralBase::AGCProceduralBase()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
	
	SetHidden(true);
#if WITH_EDITORONLY_DATA
	bRunConstructionScriptOnDrag = true;
#endif
	bRealtimeConstruction = true;
}

void AGCProceduralBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (bRealtimeConstruction || FApp::IsGame())
	{
		Construct();
	}
}
