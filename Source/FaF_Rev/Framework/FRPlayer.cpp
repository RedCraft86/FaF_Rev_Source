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

template <typename T>
T* AFRPlayer::Get(const UObject* WorldContextObject)
{
	static_assert(TIsDerivedFrom<T, ACharacter>::IsDerived, "AFRPlayer::Get can only be used to find ACharacter instances.");
	return Cast<T>(UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0));
}

ACharacter* AFRPlayer::GetPlayerPawnSmart(const UObject* WorldContextObject, const TSubclassOf<ACharacter> Class)
{
	ACharacter* Obj = UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0);
	return Obj && Obj->IsA(Class) ? Obj : nullptr;
}
