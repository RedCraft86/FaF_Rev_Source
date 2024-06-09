// Copyright (C) RedCraft86. All Rights Reserved.

#include "MeshGen/SplineWireGen.h"
#include "Components/SplineComponent.h"
#include "Components/InstancedStaticMeshComponent.h"

ASplineWireGen::ASplineWireGen()
	: SplineMeshComponents({}), AttachMeshComponents({})
	, StartMeshComponent(nullptr), EndMeshComponent(nullptr)
	, AttachmentCount(0), AttachmentLoopMode(EGeneratorLoopMode::Forward)
{
	SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	SetRootComponent(SplineComponent);
	
	SceneRoot->SetupAttachment(SplineComponent);
}

void ASplineWireGen::Construct()
{
	for (const TObjectPtr<USplineMeshComponent>& Comp : SplineMeshComponents) if (Comp) Comp->DestroyComponent();
	for (const TObjectPtr<UInstancedStaticMeshComponent>& Comp : AttachMeshComponents) if (Comp) Comp->DestroyComponent();
	if (StartMeshComponent) StartMeshComponent->DestroyComponent();
	if (EndMeshComponent) EndMeshComponent->DestroyComponent();
	
	AttachMeshComponents.Empty(AttachmentSample.Num());
	SplineMeshComponents.Empty();

	if (CableMesh.IsValidData())
	{
		for (int32 i = 0; i < SplineComponent->GetNumberOfSplinePoints() - 1; i++)
		{
			if (USplineMeshComponent* NewSplineMesh = AddComponent<USplineMeshComponent>())
			{
				UPrimitiveDataLibrary::SetStaticMeshProperties(NewSplineMesh, CableMesh);
				UPrimitiveDataLibrary::SetPrimitiveCollision(NewSplineMesh, Collision);

				FVector Location, Tangent, NextLocation, NextTangent;
				SplineComponent->GetLocalLocationAndTangentAtSplinePoint(i, Location, Tangent);
				SplineComponent->GetLocalLocationAndTangentAtSplinePoint(i + 1, NextLocation, NextTangent);
				
				NewSplineMesh->SetForwardAxis(CableMesh.MeshAxis, true);
				NewSplineMesh->SetStartAndEnd(Location, Tangent,
					NextLocation, NextTangent, true);
				
				SplineMeshComponents.Add(NewSplineMesh);
			}
		}
	}

	if (StartCap.IsValidData())
	{
		if (StartMeshComponent = AddComponent<UStaticMeshComponent>(); StartMeshComponent)
		{
			UPrimitiveDataLibrary::SetStaticMeshProperties(StartMeshComponent, StartCap);
			UPrimitiveDataLibrary::SetPrimitiveCollision(StartMeshComponent, Collision);

			FVector Location, Tangent;
			SplineComponent->GetLocalLocationAndTangentAtSplinePoint(0, Location, Tangent);
			
			StartMeshComponent->SetRelativeTransform({
				StartCap.Transform.GetRotation().Rotator() + FRotationMatrix::MakeFromX(Tangent).Rotator(),
				StartCap.Transform.GetTranslation() + Location,
				StartCap.Transform.GetScale3D()
			});
		}
	}

	if (EndCap.IsValidData())
	{
		if (EndMeshComponent = AddComponent<UStaticMeshComponent>(); EndMeshComponent)
		{
			UPrimitiveDataLibrary::SetStaticMeshProperties(EndMeshComponent, EndCap);
			UPrimitiveDataLibrary::SetPrimitiveCollision(EndMeshComponent, Collision);

			FVector Location, Tangent;
			SplineComponent->GetLocalLocationAndTangentAtSplinePoint(
				SplineComponent->GetNumberOfSplinePoints() - 1, Location, Tangent);
			
			EndMeshComponent->SetRelativeTransform({
				StartCap.Transform.GetRotation().Rotator() + FRotationMatrix::MakeFromX(Tangent).Rotator(),
				StartCap.Transform.GetTranslation() + Location,
				StartCap.Transform.GetScale3D()
			});
		}
	}

	for (int i = 0; i < AttachmentSample.Num(); i++)
	{
		UInstancedStaticMeshComponent* NewComp = AddComponent<UInstancedStaticMeshComponent>();
		UPrimitiveDataLibrary::SetStaticMeshProperties(NewComp, AttachmentSample[i]);
		UPrimitiveDataLibrary::SetPrimitiveCollision(NewComp, Collision);
		AttachMeshComponents.Add(NewComp);
	}

	if (AttachmentIndexes.Num() != AttachmentCount)
	{
		AttachmentIndexes = LoopMeshArray(AttachmentSample, AttachmentLoopMode, AttachmentCount);
	}
	
	const float Interval = SplineComponent->GetSplineLength() / (AttachmentCount + 1);
	for (int32 i = 0; i < AttachmentIndexes.Num(); i++)
	{
		const int32 idx = AttachmentIndexes[i];
		if (AttachmentSample.IsValidIndex(idx) && AttachMeshComponents.IsValidIndex(idx))
		{
			const FVector Location = SplineComponent->GetLocationAtDistanceAlongSpline(
				i * Interval, ESplineCoordinateSpace::Local);
			
			const FRotator Rotation = SplineComponent->GetRotationAtDistanceAlongSpline(
				i * Interval, ESplineCoordinateSpace::Local);

			const FTransform BaseTransform = AttachmentSample[idx].Transform;
			AttachMeshComponents[idx]->AddInstance({
				BaseTransform.GetRotation().Rotator() + Rotation,
				BaseTransform.GetTranslation() + Location,
				BaseTransform.GetScale3D()
			});
		}
	}
	
	Super::Construct();
}
#if WITH_EDITOR
void ASplineWireGen::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, AttachmentSample)
		|| PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, AttachmentLoopMode)
		|| PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, AttachmentCount))
	{
		AttachmentIndexes = LoopMeshArray(AttachmentSample, AttachmentLoopMode, AttachmentCount);
	}
}
#endif
