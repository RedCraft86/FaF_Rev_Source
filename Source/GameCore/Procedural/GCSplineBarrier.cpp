// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "GCSplineBarrier.h"
#include "Components/SplineComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "ProceduralMeshComponent.h"
#include "RCRuntimeLibrary.h"
#if WITH_EDITORONLY_DATA
#include "UObject/ConstructorHelpers.h"
#endif

AGCSplineBarrier::AGCSplineBarrier()
{
	PrimaryActorTick.bCanEverTick = false;

	SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	SplineComponent->AddSplinePoint(FVector(50.0f, 50.0f, 0.0f), ESplineCoordinateSpace::Local);
	SplineComponent->SetupAttachment(GetRootComponent());

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMesh");
	ProceduralMesh->SetupAttachment(GetRootComponent());
	
	bClosedLoop = true;
	WallHeight = 100.0f;
	Collision.SetCollisionProfile("InvisibleWall");

#if WITH_EDITORONLY_DATA
	SceneRoot->bVisualizeComponent = true;
	const ConstructorHelpers::FObjectFinder<UMaterialInterface> WireframeFinder(
		TEXT("/Game/Materials/Global/M_Wireframe.M_Wireframe"));
	if (WireframeFinder.Succeeded())
	{
		Material = WireframeFinder.Object;
	}
	const ConstructorHelpers::FObjectFinder<UMaterialInterface> OverlayFinder(
		TEXT("/Game/Materials/Global/M_Wireframe_Overlay.M_Wireframe_Overlay"));
	if (OverlayFinder.Succeeded())
	{
		OverlayMaterial = OverlayFinder.Object;
	}
#endif
}

void AGCSplineBarrier::Construct()
{
	ProceduralMesh->ClearAllMeshSections();
	ProceduralMesh->ClearCollisionConvexMeshes();

	const int PointCount = SplineComponent->GetNumberOfSplinePoints() - (SplineComponent->IsClosedLoop() ? 0 : 1);
	TArray<FVector> Vertices;
	TArray<int> Triangles;
	
	for (int i = 0; i < PointCount; i++)
	{
		const int IdxA = i;
		int IdxB = i + 1;

		if (IdxB >= PointCount && SplineComponent->IsClosedLoop())
		{
			IdxB = 0;
		}

		const FVector LocA = SplineComponent->GetLocationAtSplinePoint(IdxA, ESplineCoordinateSpace::Local);
		const FVector LocB = SplineComponent->GetLocationAtSplinePoint(IdxB, ESplineCoordinateSpace::Local);
		const int VertCount = Vertices.Num();
		
		Vertices.Append({
			{LocA.X, LocA.Y, LocA.Z},
			{LocA.X, LocA.Y, WallHeight + LocA.Z},
			{LocB.X, LocB.Y, LocB.Z},
			{LocB.X, LocB.Y, WallHeight + LocB.Z}
		});
		
		Triangles.Append({ 
			0 + VertCount, 
			1 + VertCount,
			3 + VertCount,
			3 + VertCount,
			2 + VertCount,
			0 + VertCount
		});
	}

	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, {}, Normals, Tangents);

	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, Normals, {}, {}, Tangents, true);
	URCRuntimeLibrary::SetPrimitiveCollision(ProceduralMesh, Collision);

	Super::Construct();

#if WITH_EDITORONLY_DATA
	ProceduralMesh->SetMaterial(0, Material);
	ProceduralMesh->SetMaterial(1, OverlayMaterial);
	ProceduralMesh->CreateMeshSection(1, Vertices, Triangles, Normals, {}, {}, Tangents, false);
#endif
}

#if UE_BUILD_SHIPPING
void AGCSplineBarrier::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(true);
	ProceduralMesh->SetMaterial(0, nullptr);
}
#endif

void AGCSplineBarrier::OnConstruction(const FTransform& Transform)
{
	SplineComponent->SetClosedLoop(bClosedLoop);
	const int PointCount = SplineComponent->GetNumberOfSplinePoints();
	for (int i = 0; i < PointCount; i++)
	{
		SplineComponent->SetSplinePointType(i, ESplinePointType::Linear);
	}
	
	Super::OnConstruction(Transform);
}
