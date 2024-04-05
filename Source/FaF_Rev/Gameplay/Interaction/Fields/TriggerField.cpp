// Copyright (C) RedCraft86. All Rights Reserved.

#include "TriggerField.h"
#include "Components/BrushComponent.h"
#include "Kismet/GameplayStatics.h"

ATriggerField::ATriggerField()
{
	WorldEventComponent = CreateDefaultSubobject<UWorldEventComponent>("WorldEvents");
	GetBrushComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	bSingleUse = true;
}

void ATriggerField::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (!bEnabled) return;
	if (OtherActor == UGameplayStatics::GetPlayerPawn(this, 0))
	{
		WorldEventComponent->RunEvents();
		if (bSingleUse) SetEnabled(false);
	}
}
