// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "GCSplineCable.h"
#include "RCRuntimeLibrary.h"
#include "Components/SplineComponent.h"

AGCSplineCable::AGCSplineCable()
{
	PrimaryActorTick.bCanEverTick = false;

	SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	SplineComponent->SetupAttachment(GetRootComponent());

	SetHidden(false);
#if WITH_EDITORONLY_DATA
	SceneRoot->bVisualizeComponent = true;
#endif

	Collision.SetCollisionProfile("NoCollision");
	AttachmentSpawnType = EGCProceduralSpawnType::ForwardOrder;
	AttachmentAmount = 0;
}

void AGCSplineCable::Construct()
{
	for (USplineMeshComponent* MC : SplineMeshes) { if (MC) { MC->DestroyComponent(); } }  
	SplineMeshes.Empty();

	for (UStaticMeshComponent* MC : AttachedMeshes) { if (MC) { MC->DestroyComponent(); } }  
	AttachedMeshes.Empty();
	
	if (CableMesh.IsValidMesh())
	{
		int SplinePoints = SplineComponent->GetNumberOfSplinePoints() - 1;
		for (int i = 0; i < SplinePoints; i++)
		{
			if (USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this))
			{
				SplineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
				SplineMesh->OnComponentCreated();
				SplineMesh->RegisterComponent();
				
				URCRuntimeLibrary::SetStaticMeshProperties(SplineMesh, CableMesh);
				URCRuntimeLibrary::SetPrimitiveCollision(SplineMesh, Collision);

				FVector Location, Tangent, LocationNext, TangentNext = FVector::ZeroVector;
				SplineComponent->GetLocalLocationAndTangentAtSplinePoint(i, Location, Tangent);
				SplineComponent->GetLocalLocationAndTangentAtSplinePoint(i + 1, LocationNext, TangentNext);
				
				SplineMesh->SetForwardAxis(CableMesh.MeshAxis, true);
				SplineMesh->SetStartAndEnd(Location, Tangent, LocationNext, TangentNext, true);

				SplineMesh->SetMobility(EComponentMobility::Movable);
				SplineMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
				
				SplineMeshes.Add(SplineMesh);
			}
		}
	}
	
	if (StartCap.IsValidMesh())
	{
		if (UStaticMeshComponent* StartCapMesh = NewObject<UStaticMeshComponent>(this))
		{
			StartCapMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			StartCapMesh->OnComponentCreated();
			StartCapMesh->RegisterComponent();
			
			FVector Location, Tangent = FVector::ZeroVector;
			SplineComponent->GetLocalLocationAndTangentAtSplinePoint(0, Location, Tangent);
			const FTransform Transform = {FRotationMatrix::MakeFromX(Tangent).Rotator() + StartCap.Offset.GetRotation().Rotator(),
				Location + StartCap.Offset.GetTranslation(), StartCap.Offset.GetScale3D()
			};
			
			StartCapMesh->SetRelativeTransform(Transform);
			URCRuntimeLibrary::SetStaticMeshProperties(StartCapMesh, StartCap);
			URCRuntimeLibrary::SetPrimitiveCollision(StartCapMesh, Collision);

			StartCapMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			AttachedMeshes.Add(StartCapMesh);
		}
	}

	if (EndCap.IsValidMesh())
	{
		if (UStaticMeshComponent* EndCapMesh = NewObject<UStaticMeshComponent>(this))
		{
			EndCapMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			EndCapMesh->OnComponentCreated();
			EndCapMesh->RegisterComponent();

			FVector Location, Tangent = FVector::ZeroVector;
			SplineComponent->GetLocalLocationAndTangentAtSplinePoint(SplineComponent->GetNumberOfSplinePoints() - 1, Location, Tangent);
			const FTransform Transform = {FRotationMatrix::MakeFromX(Tangent).Rotator() + EndCap.Offset.GetRotation().Rotator(),
				Location + EndCap.Offset.GetTranslation(), EndCap.Offset.GetScale3D()
			};
			
			EndCapMesh->SetRelativeTransform(Transform);
			URCRuntimeLibrary::SetStaticMeshProperties(EndCapMesh, EndCap);
			URCRuntimeLibrary::SetPrimitiveCollision(EndCapMesh, Collision);

			EndCapMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			AttachedMeshes.Add(EndCapMesh);
		}
	}
	
	if (AttachmentAmount > 0 && !AttachOptions.IsEmpty())
	{
		const float Interval = SplineComponent->GetSplineLength() / (AttachmentAmount + 1);
		for (int32 i = 1; i <= AttachmentAmount; i++)
		{
			int32 MeshIdx = 0;
			const int32 OptionCount = AttachOptions.Num();
			switch (AttachmentSpawnType)
			{
			case EGCProceduralSpawnType::ForwardOrder:
				MeshIdx = i % OptionCount;
				break;

			case EGCProceduralSpawnType::ReverseOrder:
				MeshIdx = OptionCount - (i % OptionCount) - 1;
				break;

			case EGCProceduralSpawnType::RandomOrder:
				MeshIdx = FMath::RandRange(0, OptionCount - 1);
				break;

			default: MeshIdx = 0;
			}
			
			if (!AttachOptions.IsValidIndex(MeshIdx)) { continue; }
			
			const FGCProceduralStaticMesh& Mesh = AttachOptions[MeshIdx];
			if (!Mesh.IsValidMesh()) { continue; }

			if (UStaticMeshComponent* AttachMesh = NewObject<UStaticMeshComponent>(this))
			{
				AttachMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
				AttachMesh->OnComponentCreated();
				AttachMesh->RegisterComponent();

				const FVector Location = SplineComponent->GetLocationAtDistanceAlongSpline(i * Interval, ESplineCoordinateSpace::Local);
				const FRotator Rotation = SplineComponent->GetRotationAtDistanceAlongSpline(i * Interval, ESplineCoordinateSpace::Local);
				const FTransform Transform = {Rotation + AttachTransform.GetRotation().Rotator() + Mesh.Offset.GetRotation().Rotator(),
					Location + AttachTransform.GetTranslation() + Mesh.Offset.GetTranslation(),
					Mesh.Offset.GetScale3D() * AttachTransform.GetScale3D()
				};

				AttachMesh->SetRelativeTransform(Transform);
				URCRuntimeLibrary::SetStaticMeshProperties(AttachMesh, Mesh);
				URCRuntimeLibrary::SetPrimitiveCollision(AttachMesh, Collision);

				AttachMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
				AttachedMeshes.Add(AttachMesh);
			}
		}
	}
	
	Super::Construct();
}

void AGCSplineCable::OnConstruction(const FTransform& Transform)
{
	SplineComponent->SetClosedLoop(false);
	CableMesh.FillMaterials();
	StartCap.FillMaterials();
	EndCap.FillMaterials();
	for (FRCStaticMeshProperties& MeshProp : AttachOptions)
	{
		MeshProp.FillMaterials();
	}
	
	Super::OnConstruction(Transform);
}
