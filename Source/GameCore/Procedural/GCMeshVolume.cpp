// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCMeshVolume.h"
#include "RCRuntimeLibrary.h"
#include "Components/BoxComponent.h"

AGCMeshVolume::AGCMeshVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	
	BoundingBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
	BoundingBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoundingBox->SetCollisionObjectType(ECC_WorldDynamic);
	BoundingBox->SetupAttachment(GetRootComponent());

#if WITH_EDITORONLY_DATA
	BoundingBox->SetLineThickness(0.5f);
	SceneRoot->bVisualizeComponent = true;
#endif

	MeshCount = 1;
	MeshTransform = FTransform::Identity;
	Collision.SetCollisionProfile("NoCollision");
	BoxExtent = FVector(32.0f);
}

void AGCMeshVolume::Construct()
{
	SetActorScale3D(FVector::OneVector);
	BoundingBox->SetBoxExtent({
		FMath::Max(FMath::Abs(BoxExtent.X), 4.0f),
		FMath::Max(FMath::Abs(BoxExtent.Y), 4.0f),
		FMath::Max(FMath::Abs(BoxExtent.Z), 4.0f)
	});

	StaticMeshes.Empty(MeshCount);
	for (int i = 0; i < MeshCount; i++)
	{
		const FVector RandomLocation = FMath::RandPointInBox(BoundingBox->Bounds.GetBox());
		if (UStaticMeshComponent* MeshComp = NewObject<UStaticMeshComponent>(this))
		{
			MeshComp->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			MeshComp->OnComponentCreated();
			MeshComp->RegisterComponent();
			MeshComp->SetRelativeTransform({
				MeshTransform.Rotator(),
				MeshTransform.GetTranslation() + RandomLocation,
				MeshTransform.GetScale3D()
			});
			
			URCRuntimeLibrary::SetStaticMeshProperties(MeshComp, MeshData);
			URCRuntimeLibrary::SetPrimitiveCollision(MeshComp, Collision);

			MeshComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			StaticMeshes.Add(MeshComp);
		}
	}
}
