// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCMeshVolume.h"
#include "RCRuntimeLibrary.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

AGCMeshVolume::AGCMeshVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	
	BoundingBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
	BoundingBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoundingBox->SetCollisionObjectType(ECC_WorldDynamic);
	BoundingBox->SetupAttachment(GetRootComponent());

	SetHidden(false);
#if WITH_EDITORONLY_DATA
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
	if (!MeshData.IsValidMesh()) return;
	for (int i = 0; i < MeshCount; i++)
	{
		if (UStaticMeshComponent* MeshComp = NewObject<UStaticMeshComponent>(this))
		{
			MeshComp->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			MeshComp->OnComponentCreated();
			MeshComp->RegisterComponent();
			MeshComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			MeshComp->SetWorldTransform({
				MeshTransform.Rotator() + FRotator{0.0f, FMath::RandRange(0.0f, 359.0f), 0.0f},
				MeshTransform.GetTranslation() + GetRandomLocation(),
				MeshTransform.GetScale3D()
			});
			
			URCRuntimeLibrary::SetStaticMeshProperties(MeshComp, MeshData);
			URCRuntimeLibrary::SetPrimitiveCollision(MeshComp, Collision);
			StaticMeshes.Add(MeshComp);
		}
	}
}

FVector AGCMeshVolume::GetRandomLocation() const
{
	uint8 Idx = 0;
	while (Idx < 25)
	{
		const FVector RandomLocation{UKismetMathLibrary::RandomPointInBoundingBox(
			GetActorLocation(), BoundingBox->GetUnscaledBoxExtent())};

		FVector Rand = RandomLocation;
		Rand.Z = 0.0f;

		bool HasNoOverlaps = true;
		for (const AActor* Actor : Blocking)
		{
			if (!Actor) continue;
			FVector Origin, Extent = FVector::ZeroVector;
			Actor->GetActorBounds(false, Origin, Extent);
			Origin.Z = 0.0f;
			
			if (UKismetMathLibrary::IsPointInBox(Rand, Origin, Extent))
			{
				HasNoOverlaps = false;
			}
		}

		if (HasNoOverlaps)
			return RandomLocation;
		
		Idx++;
	}
	
	return UKismetMathLibrary::RandomPointInBoundingBox(GetActorLocation(), BoundingBox->GetUnscaledBoxExtent());
}
