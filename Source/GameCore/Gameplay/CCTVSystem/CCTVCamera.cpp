// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "CCTVCamera.h"

ACCTVCamera::ACCTVCamera()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACCTVCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACCTVCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

