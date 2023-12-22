// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "CCTVCamera.h"

#include "RCRuntimeLibrary.h"

ACCTVCamera::ACCTVCamera()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool ACCTVCamera::IsOnCamera(const AActor* Actor)
{
	if (!Actor) return false;
	
	FCollisionQueryParams Params{TEXT("Camera_Actor_Check_Trace"), true, this};
	Params.AddIgnoredActor(Actor);
	
	FHitResult HitResult;
	return !GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(),
		Actor->GetActorLocation(), ECC_Visibility, Params);
}

void ACCTVCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACCTVCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

