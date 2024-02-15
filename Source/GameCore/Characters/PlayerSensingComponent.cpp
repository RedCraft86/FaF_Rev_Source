// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "PlayerSensingComponent.h"

UPlayerSensingComponent::UPlayerSensingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bDetectOnlyMovement = false;
	MinMovementSpeed = 10.0f;
	InnerAngle = 70.0f;
	OuterAngle = 80.0f;
	MaxDistance = 5000.0f;
	SenseTime = 0.25f;
	LoseTime = 1.0f;
}

void UPlayerSensingComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerSensingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
