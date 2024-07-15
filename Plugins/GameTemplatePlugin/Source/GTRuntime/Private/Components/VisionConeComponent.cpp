// Copyright (C) RedCraft86. All Rights Reserved.

#include "Components/VisionConeComponent.h"
#include "Libraries/GTRuntimeLibrary.h"

UVisionConeComponent::UVisionConeComponent()
	: TraceChannel(ECC_Visibility), MaxDistance(2000.0f), BaseAngle(50.0f), PeripheralAngle(20.0f)
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UVisionConeComponent::GetTraceTo(const AActor* InActor, const bool bCollidingOnly) const
{
	if (!InActor) return false;
	const FVector TraceStart = GetComponentLocation();

	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(InActor);
	TraceParams.AddIgnoredActor(GetOwner());
		
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart,
		InActor->GetActorLocation(), TraceChannel, TraceParams);
	if (!HitResult.bBlockingHit) return true;

	FVector Origin, BoxExtent;
	TArray<FVector> Vertices = UGTRuntimeLibrary::GetBoundingBoxVertices(InActor,
		bCollidingOnly, false, Origin, BoxExtent);
	Vertices.Add(Origin);
	for (const FVector& Vert : Vertices)
	{
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, Vert, TraceChannel, TraceParams);
		if (!HitResult.bBlockingHit) return true;
	}

	return false;
}

float UVisionConeComponent::GetAngleTo(const AActor* InActor) const
{
	if (!InActor) return -1.0f;
	const FVector DotA = InActor->GetActorLocation() - GetComponentLocation(), DotB = GetForwardVector();
	return 180.0f / UE_DOUBLE_PI * FMath::Acos(FVector::DotProduct(DotA.GetSafeNormal(), DotB.GetSafeNormal()));
}

bool UVisionConeComponent::IsActorInRange(const AActor* InActor) const
{
	if (!InActor) return false;
	return FVector::Distance(GetComponentLocation(), InActor->GetActorLocation()) <= FMath::Max(MaxDistance, 10.0f);
}

EVisionConeState UVisionConeComponent::GetActorVisionState(const AActor* InActor, const bool bCollidingOnly) const
{
	if (!IsActorInRange(InActor)) return EVisionConeState::None;

	const float AngleToTarget = GetAngleTo(InActor);
	if (AngleToTarget >= 0.0f && AngleToTarget <= GetPeripheralAngle() && GetTraceTo(InActor, bCollidingOnly))
	{
		return AngleToTarget <= GetBaseAngle() ? EVisionConeState::FullySeen : EVisionConeState::Peripheral;
	}
	
	return EVisionConeState::None;
}
