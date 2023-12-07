// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "GCVisibilityVolume.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

AGCVisibilityVolume::AGCVisibilityVolume()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.25f;

#if WITH_EDITORONLY_DATA
	bRunConstructionScriptOnDrag = true;
#endif
#if WITH_EDITOR
	RandomizeColor();
#endif

	bInvert = false;
	Targets = {};
	
	bCachedState = true;
	CachedTickStates = {};
}

#if WITH_EDITOR
void AGCVisibilityVolume::RandomizeColor()
{
#if WITH_EDITORONLY_DATA
	bColored = true;
	Color = FLinearColor::MakeRandomColor();
	BrushColor = Color.ToFColor(true);
#endif
}
#endif
#if WITH_EDITOR
void AGCVisibilityVolume::FindActors()
{
#if WITH_EDITORONLY_DATA
	Targets.Remove(nullptr);
	for (AActor* Actor : TActorRange<AActor>(GetWorld()))
	{
		if (!Actor || (bWithinBounds && !EncompassesPoint(Actor->GetActorLocation()))) continue;
		if (!FindTag.IsNone() && Actor->ActorHasTag(FindTag))
		{
			Targets.Add(Actor);
		}
	}
	Targets.Remove(nullptr);
#endif
}
#endif

void AGCVisibilityVolume::ApplyState(const bool bVisible)
{
	if (bCachedState != bVisible)
	{
		bCachedState = bVisible;
		for (AActor* Actor : Targets)
		{
			if (!Actor) continue;

			Actor->SetActorHiddenInGame(!bVisible);
			if (!bVisible)
			{
				CachedTickStates.Add(Actor, Actor->IsActorTickEnabled());
				Actor->SetActorTickEnabled(false);
			}
			else
			{
				Actor->SetActorTickEnabled(CachedTickStates.FindOrAdd(Actor, true));
			}
		}
	}
}

void AGCVisibilityVolume::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (const APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(this, 0))
	{
		const bool bOverlap = EncompassesPoint(CamManager->GetCameraLocation());
		ApplyState(bInvert ? !bOverlap : bOverlap);
	}
}
