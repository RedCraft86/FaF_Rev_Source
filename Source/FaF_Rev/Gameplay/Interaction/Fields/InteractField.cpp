// Copyright (C) RedCraft86. All Rights Reserved.

#include "InteractField.h"
#include "Components/BrushComponent.h"

AInteractField::AInteractField()
{
	WorldEventComponent = CreateDefaultSubobject<UWorldEventComponent>("WorldEvents");
	GetBrushComponent()->SetCollisionObjectType(ECC_WorldDynamic);
	GetBrushComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetBrushComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetBrushComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	bSingleUse = true;
}

void AInteractField::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (InteractionInfo.Label.IsEmptyOrWhitespace())
	{
		InteractionInfo.Label = DEFAULT_INTERACT_LABEL;
	}
}

void AInteractField::OnBeginInteract_Implementation(AFRPlayerBase* Player, const FHitResult& HitResult)
{
	if (!bEnabled) return;
	OnInteracted.Broadcast(Player);
	OnInteractedEvent.Broadcast(Player);
	WorldEventComponent->RunEvents();
	if (bSingleUse) SetEnabled(false);
}

bool AInteractField::GetInteractionInfo_Implementation(FInteractionInfo& Info)
{
	Info = InteractionInfo;
	return bEnabled;
}
