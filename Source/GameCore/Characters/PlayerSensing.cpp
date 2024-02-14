// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "PlayerSensing.h"

UPlayerSensing::UPlayerSensing()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerSensing::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerSensing::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
