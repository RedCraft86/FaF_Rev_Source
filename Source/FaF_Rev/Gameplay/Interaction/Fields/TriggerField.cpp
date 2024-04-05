// Copyright (C) RedCraft86. All Rights Reserved.

#include "TriggerField.h"
#include "FRPlayer.h"

ATriggerField::ATriggerField()
{
	WorldEventComponent = CreateDefaultSubobject<UWorldEventComponent>("WorldEvents");
	bSingleUse = true;
}

void ATriggerField::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (!bEnabled) return;
	if (OtherActor->IsA(AFRPlayer::StaticClass()))
	{
		WorldEventComponent->RunEvents();
		if (bSingleUse) SetEnabled(false);
	}
}
