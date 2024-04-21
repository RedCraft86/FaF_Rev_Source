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

AFRPlayer* AFRPlayer::GetPlayerPawnSmart(const UObject* WorldContextObject, const TSubclassOf<AFRPlayer> Class)
{
	AFRPlayer* Obj = Cast<AFRPlayer>(UGameplayStatics::GetPlayerPawn(WorldContextObject, 0));
	if (!IsValid(Obj)) Obj = Cast<AFRPlayer>(UGameplayStatics::GetActorOfClass(WorldContextObject, StaticClass()));
	return Obj && Obj->IsA(Class) ? Obj : nullptr;
}
