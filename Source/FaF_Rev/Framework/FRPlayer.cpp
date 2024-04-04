// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRPlayer.h"
#include "Kismet/GameplayStatics.h"

AFRPlayer::AFRPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFRPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFRPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFRPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

APawn* AFRPlayer::GetPlayerPawnSmart(const UObject* WorldContextObject, const TSubclassOf<APawn> Class)
{
	APawn* Obj = UGameplayStatics::GetPlayerPawn(WorldContextObject, 0);
	return Obj && Obj->IsA(Class) ? Obj : nullptr;
}
