// Copyright (C) RedCraft86. All Rights Reserved.

#include "MeshGen/SplineBarrier.h"
#include "Components/SplineComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

ASplineBarrier::ASplineBarrier() : bClosedLoop(true), WallHeight(2.0f), Mesh(nullptr), Material(nullptr)
{
	SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	SplineComponent->AddSplinePoint(FVector(50.0f, 50.0f, 0.0f), ESplineCoordinateSpace::Local);
	SplineComponent->SetMobility(EComponentMobility::Static);
	SetRootComponent(SplineComponent);

	SceneRoot->SetMobility(EComponentMobility::Static);
	SceneRoot->SetupAttachment(SplineComponent);

	WallMeshComponent = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("WallMeshComponent");
	WallMeshComponent->SetMobility(EComponentMobility::Static);
	WallMeshComponent->SetupAttachment(GetRootComponent());
	WallMeshComponent->SetLightingChannels(false, false, false);
	WallMeshComponent->bAffectDynamicIndirectLighting = false;
	WallMeshComponent->bAffectDistanceFieldLighting = false;
	WallMeshComponent->SetGenerateOverlapEvents(false);
	WallMeshComponent->SetCastShadow(false);

	SetHidden(true);
	Collision.SetProfileName(TEXT("InvisibleWall"));
#if WITH_EDITOR
	LoadObjects();
#endif
}

#if WITH_EDITOR
void ASplineBarrier::LoadObjects()
{
#if WITH_EDITORONLY_DATA
	if (!Mesh) Mesh = LoadObject<UStaticMesh>(nullptr,
		TEXT("/GameTemplatePlugin/Assets/Procedural/SM_ProcBarrier.SM_ProcBarrier"));

	if (!Material) Material = LoadObject<UMaterialInterface>(nullptr,
		TEXT("/GameTemplatePlugin/Assets/Procedural/M_ProcBarrier_01.M_ProcBarrier_01"));

	if (!OverlayMaterial) OverlayMaterial = LoadObject<UMaterialInterface>(nullptr,
		TEXT("/GameTemplatePlugin/Assets/Procedural/M_ProcBarrier_02.M_ProcBarrier_02"));
#endif
}
#endif

void ASplineBarrier::BeginPlay()
{
	Super::BeginPlay();
	WallMeshComponent->SetOverlayMaterial(nullptr);
}

void ASplineBarrier::Construct()
{
#if WITH_EDITOR
	LoadObjects();
#endif
	SplineComponent->SetClosedLoop(bClosedLoop);
	const int PointCount = SplineComponent->GetNumberOfSplinePoints();
	for (int i = 0; i < PointCount; i++) SplineComponent->SetSplinePointType(i, ESplinePointType::Linear);
	const int GenPointCount = PointCount - (bClosedLoop ? 0 : 1);
	
	WallMeshComponent->ClearInstances();
	WallMeshComponent->SetStaticMesh(Mesh);
	WallMeshComponent->SetMaterial(0, Material);
	UPrimitiveDataLibrary::SetPrimitiveCollision(WallMeshComponent, Collision);
	
	for (int32 i = 0; i < GenPointCount; i++)
	{
		if (SkipIndexes.Contains(i)) continue;
		
		FVector Location, Tangent;
		SplineComponent->GetLocalLocationAndTangentAtSplinePoint(i, Location, Tangent);
		const float DistB = SplineComponent->GetDistanceAlongSplineAtSplinePoint(i + 1);
		const float DistA = SplineComponent->GetDistanceAlongSplineAtSplinePoint(i);

		WallMeshComponent->AddInstance({
			Tangent.Rotation(), Location,
			{(DistB - DistA) / 100.0f, 1.0f, WallHeight}
		});
	}
	
#if WITH_EDITORONLY_DATA
	WallMeshComponent->SetOverlayMaterial(OverlayMaterial);
#endif
	Super::Construct();
}
