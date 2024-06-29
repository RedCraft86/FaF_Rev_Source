// Copyright (C) RedCraft86. All Rights Reserved.

#include "MeshGen/SplinePolygon.h"
#include "Components/SplineComponent.h"
#include "CompGeom/PolygonTriangulation.h"
#include "IndexTypes.h"

ASplinePolygon::ASplinePolygon() : Material(nullptr)
{
	SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	SplineComponent->AddSplinePoint(FVector(50.0f, 50.0f, 0.0f), ESplineCoordinateSpace::Local);
	SetRootComponent(SplineComponent);
	
	SceneRoot->SetupAttachment(SplineComponent);

	SetHidden(true);
	Collision.SetProfileName(TEXT("InvisibleWallDynamic"));
#if WITH_EDITORONLY_DATA
	const ConstructorHelpers::FObjectFinder<UMaterialInterface> WireframeFinder(
		TEXT("/GameTemplatePlugin/Assets/Procedural/M_ProcBarrier_01.M_ProcBarrier_01"));
	if (WireframeFinder.Succeeded()) Material = WireframeFinder.Object;
	
	const ConstructorHelpers::FObjectFinder<UMaterialInterface> OverlayFinder(
		TEXT("/GameTemplatePlugin/Assets/Procedural/M_ProcBarrier_02.M_ProcBarrier_02"));
	if (OverlayFinder.Succeeded()) OverlayMaterial = OverlayFinder.Object;
#endif
}

#if WITH_EDITORONLY_DATA
void ASplinePolygon::BeginPlay()
{
	Super::BeginPlay();
	ProceduralMesh->ClearMeshSection(1);
}
#endif

void ASplinePolygon::Construct()
{
	SplineComponent->SetClosedLoop(true);
	const int PointCount = SplineComponent->GetNumberOfSplinePoints();
	for (int i = 0; i < PointCount; i++) SplineComponent->SetSplinePointType(i, ESplinePointType::Linear);
	
	ProceduralMesh->ClearAllMeshSections();
	ProceduralMesh->ClearCollisionConvexMeshes();
	ProceduralMesh->bUseComplexAsSimpleCollision = true;
	
	TArray<FVector> Vertices;
	for (int i = 0; i < PointCount; i++)
	{
		const FVector Vert = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector UpVector = SplineComponent->GetUpVectorAtSplinePoint(i, ESplineCoordinateSpace::Local);
		
		Vertices.Add(Vert);
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

	UPrimitiveDataLibrary::SetPrimitiveCollision(ProceduralMesh, Collision);
	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, Normals, {}, {}, Tangents, false);
	ProceduralMesh->SetMaterial(0, Material);

#if WITH_EDITORONLY_DATA
	if (OverlayMaterial)
	{
		ProceduralMesh->CreateMeshSection(1, Vertices, Triangles, Normals, {}, {}, Tangents, false);
		ProceduralMesh->SetMaterial(1, OverlayMaterial);
	}
#endif
	
	Super::Construct();
}
