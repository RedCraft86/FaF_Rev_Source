// Copyright (C) RedCraft86. All Rights Reserved.

#include "InteractField.h"
#include "Components/BrushComponent.h"

AInteractField::AInteractField()
{
	WorldEventComponent = CreateDefaultSubobject<UWorldEventComponent>("WorldEvents");
	GetBrushComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	bSingleUse = true;
	DisplayLabel = DEFAULT_INTERACT_LABEL;
}

void AInteractField::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (DisplayLabel.IsEmptyOrWhitespace())
	{
		DisplayLabel = DEFAULT_INTERACT_LABEL;
	}
}

void AInteractField::OnBeginInteract_Implementation(AFRPlayerBase* Player, const FHitResult& HitResult)
{
	if (!bEnabled) return;
	WorldEventComponent->RunEvents();
	if (bSingleUse) SetEnabled(false);
}

bool AInteractField::GetInteractionInfo_Implementation(FInteractionInfo& Info)
{
	Info.Label = DisplayLabel;
	Info.Icon = nullptr;
	return bEnabled;
}
