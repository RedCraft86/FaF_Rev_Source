// Copyright (C) RedCraft86. All Rights Reserved.

#include "InteractField.h"

AInteractField::AInteractField()
{
	CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
	CollisionBox->SetupAttachment(GetRootComponent());
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);

	WorldEventComponent = CreateDefaultSubobject<UWorldEventComponent>("WorldEvents");
	
	bSingleUse = true;
	DisplayLabel = DefaultInteractionLabel;
}

void AInteractField::OnEnableStateChange(const bool bEnabled)
{
	SetActorEnableCollision(bEnabled);
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
