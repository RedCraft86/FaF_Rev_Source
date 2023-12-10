// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "GCCameraSmoother.h"

const FName UGCCameraSmoother::SocketName(TEXT("CameraSmoother"));

UGCCameraSmoother::UGCCameraSmoother()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	bEnabled = true;
	LagSpeed = 10.0f;
	PreviousDesiredRotation = {};
	RelativeSocketRotation = {};
}

void UGCCameraSmoother::SetSmootherEnabled(const bool bInEnabled)
{
	if (bEnabled != bInEnabled)
	{
		bEnabled = bInEnabled;
		if (!bEnabled)
		{
			OverrideWorldRotation(GetDesiredRotation());
		}
	}
}

void UGCCameraSmoother::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bEnabled)
		return;
	
	FRotator DesiredRotation = GetDesiredRotation();
	if (!bIsLockOn && LagSpeed > 0.0f)
	{
		DesiredRotation = FMath::RInterpTo(PreviousDesiredRotation, DesiredRotation, DeltaTime, LagSpeed);
		PreviousDesiredRotation = DesiredRotation;

		RelativeSocketRotation = GetComponentTransform().InverseTransformRotation(DesiredRotation.Quaternion()).Rotator();
	}
	else
	{
		PreviousDesiredRotation = DesiredRotation;
		RelativeSocketRotation = GetComponentTransform().InverseTransformRotation(DesiredRotation.Quaternion()).Rotator();
	}

	UpdateChildTransforms();
}

void UGCCameraSmoother::QuerySupportedSockets(TArray<FComponentSocketDescription>& OutSockets) const
{
	new (OutSockets) FComponentSocketDescription(SocketName, EComponentSocketType::Socket);
}

FTransform UGCCameraSmoother::GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace) const
{
	const FTransform RelativeTransform(RelativeSocketRotation);

	switch (TransformSpace)
	{
	case RTS_World:
		return RelativeTransform * GetComponentTransform();
		
	case RTS_Actor:
		if (const AActor* Actor = GetOwner())
		{
			const FTransform SocketTransform = RelativeTransform * GetComponentTransform();
			return SocketTransform.GetRelativeTransform(Actor->GetTransform());
		}
		break;
		
	case RTS_Component:
		return RelativeTransform;
		
	default: break;
	}
	
	return RelativeTransform;
}

FRotator UGCCameraSmoother::GetDesiredRotation() const
{
	if (const APawn* OwningPawn = Cast<APawn>(GetOwner()))
	{
		return OwningPawn->GetViewRotation();
	}

	return GetComponentRotation();
}

void UGCCameraSmoother::OverrideRelativeRotation(const FRotator NewRotation)
{
	SetRelativeRotation(NewRotation);
	RelativeSocketRotation = NewRotation;
	PreviousDesiredRotation = GetComponentTransform().TransformRotation(NewRotation.Quaternion()).Rotator();
}

void UGCCameraSmoother::OverrideWorldRotation(const FRotator NewRotation)
{
	SetWorldRotation(NewRotation);
	PreviousDesiredRotation = NewRotation;
	RelativeSocketRotation = GetComponentTransform().InverseTransformRotation(NewRotation.Quaternion()).Rotator();
}
