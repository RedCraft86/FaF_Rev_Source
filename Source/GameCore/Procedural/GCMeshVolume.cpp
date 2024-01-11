// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCMeshVolume.h"
#include "RCRuntimeLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"

AGCMeshVolume::AGCMeshVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
	
	BoundingBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
	BoundingBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoundingBox->SetCollisionObjectType(ECC_WorldDynamic);
	BoundingBox->SetupAttachment(GetRootComponent());

	InstancedMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>("StaticMeshes");
	BoundingBox->SetupAttachment(GetRootComponent());

#if WITH_EDITORONLY_DATA
	BoundingBox->SetLineThickness(0.5f);
	SceneRoot->bVisualizeComponent = true;
#endif

	MeshCount = 1;
	MeshTransform = FTransform::Identity;
	Collision.SetCollisionProfile("NoCollision");
}

void AGCMeshVolume::Construct()
{
	InstancedMeshes->ClearInstances();
	URCRuntimeLibrary::SetStaticMeshProperties(InstancedMeshes, MeshData);
	URCRuntimeLibrary::SetPrimitiveCollision(InstancedMeshes, Collision);
	for (int i = 0; i < MeshCount; i++)
	{
		const FVector RandomLocation = FMath::RandPointInBox(BoundingBox->Bounds.GetBox());
		InstancedMeshes->AddInstance({
			MeshTransform.GetRotation(),
			MeshTransform.GetTranslation() + RandomLocation,
			MeshTransform.GetScale3D()
		});
	}
}
