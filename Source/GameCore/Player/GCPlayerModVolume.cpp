// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCPlayerModVolume.h"
#include "GCPlayerCharacter.h"

AGCPlayerModVolume::AGCPlayerModVolume()
{
	PrimaryActorTick.bCanEverTick = false;
}

AGCPlayerVolume_Speed::AGCPlayerVolume_Speed()
{
	SpeedMultiplier = 0.5f;
}

void AGCPlayerVolume_Speed::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (AGCPlayerCharacter* Player = Cast<AGCPlayerCharacter>(OtherActor))
	{
		Player->AddWalkMultiplierModifier(GetFName(), SpeedMultiplier);
	}
}

void AGCPlayerVolume_Speed::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	if (AGCPlayerCharacter* Player = Cast<AGCPlayerCharacter>(OtherActor))
	{
		Player->RemoveWalkMultiplierModifier(GetFName());
	}
}
