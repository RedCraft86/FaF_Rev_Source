// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCSplinePolygon.h"
#include "Components/SplineComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "CompGeom/PolygonTriangulation.h"
#include "ProceduralMeshComponent.h"
#include "RCRuntimeLibrary.h"
#if WITH_EDITORONLY_DATA
#include "UObject/ConstructorHelpers.h"
#endif

AGCSplinePolygon::AGCSplinePolygon()
{
	PrimaryActorTick.bCanEverTick = false;

	SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	SplineComponent->AddSplinePoint(FVector(50.0f, 50.0f, 0.0f), ESplineCoordinateSpace::Local);
	SplineComponent->SetupAttachment(GetRootComponent());

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMesh");
	ProceduralMesh->SetupAttachment(GetRootComponent());
	
	Collision.ObjectType = ECC_WorldStatic;
	Collision.CollisionEnabled = ECollisionEnabled::QueryOnly;
	Collision.SetCollisionProfile("BlockAll");

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

void AGCSplinePolygon::Construct()
{
	ProceduralMesh->ClearAllMeshSections();
	ProceduralMesh->ClearCollisionConvexMeshes();
	
	TArray<FVector> Vertices;
	const int PointCount = SplineComponent->GetNumberOfSplinePoints();
	for (int i = 0; i < PointCount; i++)
	{
		Vertices.Add(SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local));
	}

	TArray<int> Triangles;
	TArray<UE::Geometry::FIndex3i> Tris;
	PolygonTriangulation::TriangulateSimplePolygon(Vertices, Tris);
	for (const UE::Geometry::FIndex3i& Triangle : Tris)
	{
		Triangles.Add(Triangle.A);
		Triangles.Add(Triangle.B);
		Triangles.Add(Triangle.C);
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
void AGCSplinePolygon::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(true);
	ProceduralMesh->SetMaterial(0, nullptr);
}
#endif

void AGCSplinePolygon::OnConstruction(const FTransform& Transform)
{
	SplineComponent->SetClosedLoop(true);
	const int PointCount = SplineComponent->GetNumberOfSplinePoints();
	for (int i = 0; i < PointCount; i++)
	{
		const FVector Loc = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		SplineComponent->SetLocationAtSplinePoint(i, { Loc.X, Loc.Y, 0.0f }, ESplineCoordinateSpace::Local);
		SplineComponent->SetSplinePointType(i, ESplinePointType::Linear);
	}
	
	Super::OnConstruction(Transform);
}
