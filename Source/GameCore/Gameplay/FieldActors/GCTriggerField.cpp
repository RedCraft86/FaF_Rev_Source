// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "GCTriggerField.h"
#include "RCRuntimeLibrary.h"
#include "RCUtilsSubsystem.h"
#include "Gameplay/GCFieldUtils.h"
#include "Components/BoxComponent.h"

AGCTriggerField::AGCTriggerField()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetupAttachment(GetRootComponent());

#if WITH_EDITORONLY_DATA
	CollisionBox->SetLineThickness(0.5f);
	SceneRoot->bVisualizeComponent = true;
#endif

	bStartEnabled = true;
	bSingleUse = true;
	BoxExtent = FVector(32.0f);
	Collision.SetAllResponses(ECR_Ignore);
	Collision.SetResponse(ECC_Pawn, ECR_Overlap);
	Collision.CollisionEnabled = ECollisionEnabled::QueryOnly;
	Collision.ObjectType = ECC_WorldDynamic;
	Events = {};
}

void AGCTriggerField::SetEnabled(const bool bEnabled)
{
	SetActorEnableCollision(bEnabled);
	SetActorHiddenInGame(!bEnabled);
	if (bEnabled)
	{
		bUsed = false;
	}
}

void AGCTriggerField::OnConstruction(const FTransform& Transform)
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

void AGCTriggerField::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!bUsed && OtherActor->IsA<AGCPlayerCharacter>())
	{
		OnTriggered.Broadcast();
		OnTriggeredBP.Broadcast();
	
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
}
