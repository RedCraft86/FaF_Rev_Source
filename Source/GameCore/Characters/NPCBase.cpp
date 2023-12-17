// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "NPCBase.h"

ANPCBase::ANPCBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ANPCBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ANPCBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
