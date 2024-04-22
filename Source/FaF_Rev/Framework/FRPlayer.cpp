// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRPlayer.h"
#include "Kismet/GameplayStatics.h"

AFRPlayerBase::AFRPlayerBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFRPlayerBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFRPlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFRPlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

/* Statics */

AFRPlayerBase* AFRPlayerBase::GetPlayerPawnSmart(const UObject* WorldContextObject, const TSubclassOf<AFRPlayerBase> Class)
{
	AFRPlayerBase* Obj = Cast<AFRPlayerBase>(UGameplayStatics::GetPlayerPawn(WorldContextObject, 0));
	if (!IsValid(Obj)) Obj = Cast<AFRPlayerBase>(UGameplayStatics::GetActorOfClass(WorldContextObject, StaticClass()));
	return Obj && Obj->IsA(Class) ? Obj : nullptr;
}
