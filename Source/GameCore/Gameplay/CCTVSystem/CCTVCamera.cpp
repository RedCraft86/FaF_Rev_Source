// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "CCTVCamera.h"

#include "RCRuntimeLibrary.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/KismetMathLibrary.h"

ACCTVCamera::ACCTVCamera()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
	
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCapture");
	SceneCapture->SetupAttachment(GetRootComponent());
	SceneCapture->bAlwaysPersistRenderingState = true;
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bCaptureOnMovement = false;
	SceneCapture->FOVAngle = 80.0f;

	LookTarget = {50.0f, 50.0f, -25.0f};
	MinXY = {-25.0f, -25.0f};
	MaxXY = {25.0f, 25.0f};
	BaseOffset = FRotator::ZeroRotator;
	CurrentOffset = FVector2D::ZeroVector;
}

void ACCTVCamera::SetCameraRotation_Implementation(const FRotator& Rotation)
{
	SceneCapture->SetRelativeRotation(Rotation);
}

void ACCTVCamera::SetState(const bool bActive)
{
	SetActorTickEnabled(bActive);
}

bool ACCTVCamera::IsActorOnCamera(const AActor* Actor)
{
	if (!Actor) return false;
	
	FCollisionQueryParams Params{TEXT("Camera_Actor_Check_Trace"), true, this};
	Params.AddIgnoredActor(Actor);

	if (!URCRuntimeLibrary::IsActorInFront(this, Actor))
	{
		return false;
	}
	
	FHitResult HitResult;
	return !GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(),
		Actor->GetActorLocation(), ECC_Visibility, Params);
}

void ACCTVCamera::AddTurnInput(const float X, const float Y)
{
	CurrentOffset.X = FMath::Clamp(CurrentOffset.X + X, MinXY.X, MaxXY.X);
	CurrentOffset.Y = FMath::Clamp(CurrentOffset.Y + Y, MinXY.Y, MaxXY.Y);
	SetCameraRotation(BaseOffset + FRotator{CurrentOffset.Y, CurrentOffset.X, 0.0f});
}

void ACCTVCamera::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	SceneCapture->CaptureScene();
}

void ACCTVCamera::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (USceneComponent* AttachRoot = GetAttachmentRoot())
	{
		SceneCapture->AttachToComponent(AttachRoot, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	else
	{
		SceneCapture->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	}

	BaseOffset = GetActorTransform().InverseTransformRotation(
		UKismetMathLibrary::FindLookAtRotation(SceneCapture->GetComponentLocation(),
		GetActorTransform().TransformPosition(LookTarget)).Quaternion()).Rotator();
	SetCameraRotation(BaseOffset);
}
