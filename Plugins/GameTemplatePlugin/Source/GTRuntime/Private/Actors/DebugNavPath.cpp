// Copyright (C) RedCraft86. All Rights Reserved.

#include "Actors/DebugNavPath.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

ADebugNavPath::ADebugNavPath()
{
	PrimaryActorTick.bCanEverTick = false;
	bIsEditorOnlyActor = true;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

	ShapeComponent = CreateDefaultSubobject<UDebugShapesComponent>("ShapeComponent");
	
#if WITH_EDITORONLY_DATA
	SceneRoot->bVisualizeComponent = true;
	bRunConstructionScriptOnDrag = true;
#endif
}

void ADebugNavPath::BeginPlay()
{
	Super::BeginPlay();
	K2_DestroyActor();
}

void ADebugNavPath::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
#if WITH_EDITORONLY_DATA
	ShapeComponent->DebugLines.Empty();
	ShapeComponent->DebugSpheres.Empty();
	ShapeComponent->DebugStrings.Empty();
	if (ADebugNavPath* PathPtr = NextPath.LoadSynchronous())
	{
		if (const UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(
			this, GetActorLocation(), PathPtr, 0.0f))
		{
			const TArray<FVector> Points = NavPath->PathPoints;
			for (int i = 0; i < Points.Num(); i++)
			{
				FDebugSphereData SD; SD.Color = Color;
				SD.Location = GetActorTransform().InverseTransformPosition(Points[i]);
				ShapeComponent->DebugSpheres.Add(*LexToString(i), SD);
				if (i < Points.Num() - 1)
				{
					FDebugLineData LD;
					LD.Color = Color;
					LD.Start = SD.Location;
					LD.End = GetActorTransform().InverseTransformPosition(Points[i + 1]);
					ShapeComponent->DebugLines.Add(*LexToString(i), LD);
				}
			}

			FDebugStringData StrData; StrData.Color = Color;
			StrData.String = FString::Printf(TEXT("Length: %.2f"), NavPath->GetPathLength());
			ShapeComponent->DebugStrings.Add("PathLength", StrData);
		}
	}
#endif
}
