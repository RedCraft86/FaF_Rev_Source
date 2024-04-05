// Copyright (C) RedCraft86. All Rights Reserved.

#include "InteractField.h"
#include "Components/BrushComponent.h"

AInteractField::AInteractField()
{
	WorldEventComponent = CreateDefaultSubobject<UWorldEventComponent>("WorldEvents");
	GetBrushComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	bSingleUse = true;
	DisplayLabel = DefaultInteractionLabel;
}

void AInteractField::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (DisplayLabel.IsEmptyOrWhitespace())
	{
		DisplayLabel = DefaultInteractionLabel;
	}
}

void AInteractField::OnBeginInteract_Implementation(AFRPlayer* Player, const FHitResult& HitResult)
{
	if (!bEnabled) return;
	
	WorldEventComponent->RunEvents();
	if (bSingleUse) SetEnabled(false);
}

bool AInteractField::GetInteractionInfo_Implementation(FText& DisplayName)
{
	DisplayName = DisplayLabel;
	return bEnabled;
}
