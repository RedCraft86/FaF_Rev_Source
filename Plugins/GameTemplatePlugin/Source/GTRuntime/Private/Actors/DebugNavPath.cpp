// Copyright (C) RedCraft86. All Rights Reserved.

#include "Actors/DebugNavPath.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

ADebugNavPath::ADebugNavPath()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
	bIsEditorOnlyActor = true;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
#if WITH_EDITORONLY_DATA
	SceneRoot->bVisualizeComponent = true;
	bRunConstructionScriptOnDrag = true;
#endif
}

void ADebugNavPath::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
	K2_DestroyActor();
}

void ADebugNavPath::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
#if WITH_EDITORONLY_DATA
	if (ADebugNavPath* PathPtr = NextPath.LoadSynchronous())
	{
		if (const UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(
			this, GetActorLocation(), PathPtr, 0.0f))
		{
			const TArray<FVector> Points = NavPath->PathPoints;
			for (int i = 0; i < Points.Num(); i++)
			{
				DrawDebugSphere(GetWorld(), Points[i], 16, 8, Color, false,
					PrimaryActorTick.TickInterval, 0, 1.0f);
				if (i < Points.Num() - 1)
				{
					DrawDebugLine(GetWorld(), Points[i], Points[i + 1], Color, false,
						PrimaryActorTick.TickInterval, 0, 1.0f);
				}
			}

			DrawDebugString(GetWorld(), GetActorLocation(), FString::Printf(TEXT("Length: %.2f"), NavPath->GetPathLength()),
				this, Color, PrimaryActorTick.TickInterval, true, 12);
		}
	}
#endif
}