// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCBoxInteract.h"
#include "RCRuntimeLibrary.h"
#include "Components/BoxComponent.h"
#if WITH_EDITOR
#include "Components/BillboardComponent.h"
#endif

AGCBoxInteract::AGCBoxInteract()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetupAttachment(SceneRoot);

#if WITH_EDITORONLY_DATA
	CollisionBox->SetLineThickness(1.0f);
	CollisionBox->ShapeColor = FColor(250, 140, 250, 255);
	SceneRoot->bVisualizeComponent = false;
#endif
#if WITH_EDITOR
	VisualIcon = CreateEditorOnlyDefaultSubobject<UBillboardComponent>("VisualIcon");
	if (VisualIcon)
	{
		VisualIcon->SetRelativeScale3D_Direct(FVector(0.5f, 0.5f, 0.5f));
		VisualIcon->bIsScreenSizeScaled = true;
		VisualIcon->SetupAttachment(GetRootComponent());
		ConstructorHelpers::FObjectFinder<UTexture2D> IconFinder(TEXT("/Engine/EditorResources/S_Trigger.S_Trigger"));
		if (IconFinder.Succeeded())
		{
			VisualIcon->Sprite = IconFinder.Object;
		}
	}
#endif
	
	bSingleUse = true;
	Label = FText::FromString(TEXT("Interact"));
	BoxExtent = FVector(32.0f);
	Collision.SetAllResponses(ECR_Ignore);
	Collision.SetResponse(ECC_GameTraceChannel1, ECR_Block);
	Collision.CollisionEnabled = ECollisionEnabled::QueryOnly;
	Collision.ObjectType = ECC_WorldDynamic;
}

void AGCBoxInteract::LockTask()
{
	Super::LockTask();
	CollisionBox->Deactivate();
}

void AGCBoxInteract::UnlockTask()
{
	Super::UnlockTask();
	CollisionBox->Activate();
}

void AGCBoxInteract::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	URCRuntimeLibrary::SetPrimitiveCollision(CollisionBox, Collision);
	CollisionBox->SetBoxExtent({
		FMath::Max(FMath::Abs(BoxExtent.X), 4.0f),
		FMath::Max(FMath::Abs(BoxExtent.Y), 4.0f),
		FMath::Max(FMath::Abs(BoxExtent.Z), 4.0f)
	});
}

void AGCBoxInteract::OnBeginInteract_Implementation(AGCPlayerCharacter* Player, const FHitResult& HitResult)
{
	CompleteTask();
}

bool AGCBoxInteract::GetInteractionInfo_Implementation(FText& DisplayName)
{
	DisplayName = Label;
	return true;
}
