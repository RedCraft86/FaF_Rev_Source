// Copyright (C) RedCraft86. All Rights Reserved.

#include "Components/DebugShapesComponent.h"

UDebugShapesComponent::UDebugShapesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bIsEditorOnly = true;
#if WITH_EDITORONLY_DATA
	SetIsVisualizationComponent(true);
#endif
}

#if WITH_EDITORONLY_DATA
FVector UDebugShapesComponent::TransformLocation(const FVector& LocalLocation) const
{
	return GetOwner()->GetActorTransform().TransformPosition(LocalLocation);
}

FRotator UDebugShapesComponent::TransformRotation(const FRotator& LocalRotation) const
{
	return GetOwner()->GetActorTransform().TransformRotation(LocalRotation.Quaternion()).Rotator();
}

FVector UDebugShapesComponent::AsForwardVector(const FRotator& LocalRotation) const
{
	return TransformRotation(LocalRotation).Vector();
}

FVector UDebugShapesComponent::AsRightVector(const FRotator& LocalRotation) const
{
	return FRotationMatrix(TransformRotation(LocalRotation)).GetScaledAxis(EAxis::Y);
}

FVector UDebugShapesComponent::AsUpVector(const FRotator& LocalRotation) const
{
	return FRotationMatrix(TransformRotation(LocalRotation)).GetScaledAxis(EAxis::Z);
}
#endif

void UDebugShapesComponent::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimerForNextTick([WeakThis = TWeakObjectPtr<ThisClass>(this)]()
	{
		if (WeakThis.IsValid()) WeakThis->DestroyComponent();
	});
}
