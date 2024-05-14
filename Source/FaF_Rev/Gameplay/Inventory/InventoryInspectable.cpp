// Copyright (C) RedCraft86. All Rights Reserved.

#include "InventoryInspectable.h"

AInventoryInspectable::AInventoryInspectable()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AInventoryInspectable::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInventoryInspectable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
