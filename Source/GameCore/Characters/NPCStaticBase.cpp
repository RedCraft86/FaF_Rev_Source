// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "NPCStaticBase.h"

ANPCStaticBase::ANPCStaticBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ANPCStaticBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ANPCStaticBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
