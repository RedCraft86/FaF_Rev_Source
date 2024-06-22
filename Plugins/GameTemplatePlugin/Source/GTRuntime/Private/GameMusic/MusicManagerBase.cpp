// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameMusic/MusicManagerBase.h"

AMusicManagerBase::AMusicManagerBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMusicManagerBase::BeginPlay()
{
	Super::BeginPlay();
}
