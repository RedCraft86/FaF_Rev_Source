// Copyright (C) RedCraft86. All Rights Reserved.

#include "InventoryInspection.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SpotLightComponent.h"

AInventoryInspection::AInventoryInspection() : TurnSpeed(5.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
#if WITH_EDITOR
	SceneRoot->bVisualizeComponent = true;
#endif

	InspectionRoot = CreateDefaultSubobject<USpringArmComponent>("InspectionRoot");
	InspectionRoot->SetupAttachment(SceneRoot);
	InspectionRoot->bEnableCameraRotationLag = true;
	InspectionRoot->bDoCollisionTest = false;
	InspectionRoot->TargetArmLength = 0.0f;

	InspectionMesh = CreateDefaultSubobject<UStaticMeshComponent>("InspectionMesh");
	InspectionMesh->SetLightingChannels(false, true, false);
	InspectionMesh->SetupAttachment(SceneRoot);

	InspectionLight = CreateDefaultSubobject<USpotLightComponent>("InspectionLight");
	InspectionLight->SetLightingChannels(false, true, false);
	InspectionLight->SetupAttachment(SceneRoot);
	
	InspectionCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("InspectionCapture");
	InspectionCapture->SetupAttachment(SceneRoot);
}

void AInventoryInspection::SetMeshData(const FStaticMeshProperties& InProperties, const FTransform& RelativeTransform) const
{
	UPrimitiveDataLibrary::SetStaticMeshProperties(InspectionMesh, InProperties);
	InspectionMesh->SetRelativeTransform(RelativeTransform);
	
	InspectionRoot->bEnableCameraRotationLag = false;
	InspectionRoot->SetRelativeRotation(FRotator::ZeroRotator);
	InspectionRoot->bEnableCameraRotationLag = true;
}

void AInventoryInspection::BeginPlay()
{
	Super::BeginPlay();
	InspectionCapture->SetComponentTickEnabled(false);
}

void AInventoryInspection::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	InspectionLight->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(
		InspectionLight->GetComponentLocation(), InspectionRoot->GetComponentLocation()));

	InspectionRoot->CameraRotationLagSpeed = TurnSpeed;
	InspectionCapture->ShowOnlyComponent(InspectionMesh);
	InspectionCapture->ShowOnlyActors.AddUnique(this);
}
