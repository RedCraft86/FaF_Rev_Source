// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "GCInteractField.h"
#include "RCRuntimeLibrary.h"
#include "RCUtilsSubsystem.h"
#include "Gameplay/GCFieldUtils.h"
#include "Components/BoxComponent.h"

AGCInteractField::AGCInteractField()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetupAttachment(GetRootComponent());

#if WITH_EDITORONLY_DATA
	CollisionBox->SetLineThickness(0.5f);
	SceneRoot->bVisualizeComponent = true;
#endif

	bStartEnabled = true;
	bSingleUse = true;
	Label = FText::FromString("Interact");
	BoxExtent = FVector(32.0f);
	Collision.SetAllResponses(ECR_Ignore);
	Collision.SetResponse(ECC_GameTraceChannel1, ECR_Block);
	Collision.CollisionEnabled = ECollisionEnabled::QueryOnly;
	Collision.ObjectType = ECC_WorldDynamic;
	Events = {};
}

void AGCInteractField::SetEnabled(const bool bEnabled)
{
	SetActorEnableCollision(bEnabled);
	SetActorHiddenInGame(!bEnabled);
	if (bEnabled)
	{
		bUsed = false;
	}
}

void AGCInteractField::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	URCRuntimeLibrary::SetPrimitiveCollision(CollisionBox, Collision);
	CollisionBox->SetBoxExtent({
		FMath::Max(FMath::Abs(BoxExtent.X), 4.0f),
		FMath::Max(FMath::Abs(BoxExtent.Y), 4.0f),
		FMath::Max(FMath::Abs(BoxExtent.Z), 4.0f)
	});

	SetEnabled(bStartEnabled);
}

void AGCInteractField::OnBeginInteract_Implementation(AGCPlayerCharacter* Player, const FHitResult& HitResult)
{
	if (bUsed)
	{
		return;
	}
	
	OnInteracted.Broadcast();
	OnInteractedBP.Broadcast();
	
	for (FInstancedStruct& Event : Events)
	{
		if (FGCEvent* EventPtr = Event.GetMutablePtr<FGCEvent>())
		{
			EventPtr->RunEvent(this);
		}
	}
	
	if (bSingleUse)
	{
		bUsed = true;
		SetEnabled(false);
	}
}

bool AGCInteractField::GetInteractionInfo_Implementation(FText& DisplayName)
{
	DisplayName = Label;
	return !bUsed;
}
