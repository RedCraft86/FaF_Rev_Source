// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "GCBoxTrigger.h"
#include "Components/BoxComponent.h"
#include "Player/GCPlayerCharacter.h"
#if WITH_EDITOR
#include "Components/BillboardComponent.h"
#endif

AGCBoxTrigger::AGCBoxTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetupAttachment(SceneRoot);

#if WITH_EDITORONLY_DATA
	CollisionBox->SetLineThickness(1.0f);
	CollisionBox->ShapeColor = FColor(150, 255, 150, 255);
	SceneRoot->bVisualizeComponent = false;
#endif
#if WITH_EDITOR
	VisualIcon = CreateEditorOnlyDefaultSubobject<UBillboardComponent>("VisualIcon");
	if (VisualIcon)
	{
		VisualIcon->SetRelativeScale3D_Direct(FVector(0.5f, 0.5f, 0.5f));
		VisualIcon->bIsScreenSizeScaled = true;
		VisualIcon->SetupAttachment(GetRootComponent());
		ConstructorHelpers::FObjectFinder<UTexture2D> IconFinder(TEXT("/Engine/EditorResources/S_TriggerBox.S_TriggerBox"));
		if (IconFinder.Succeeded())
		{
			VisualIcon->Sprite = IconFinder.Object;
		}
	}
#endif
	
	bSingleUse = true;
	BoxExtent = FVector(32.0f);
}

void AGCBoxTrigger::LockTask()
{
	Super::LockTask();
	CollisionBox->Deactivate();
}

void AGCBoxTrigger::UnlockTask()
{
	Super::UnlockTask();
	CollisionBox->Activate();
}

void AGCBoxTrigger::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	CollisionBox->SetBoxExtent({
		FMath::Max(FMath::Abs(BoxExtent.X), 4.0f),
		FMath::Max(FMath::Abs(BoxExtent.Y), 4.0f),
		FMath::Max(FMath::Abs(BoxExtent.Z), 4.0f)
	});
}

void AGCBoxTrigger::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	// const APawn* AsPawn = Cast<APawn>(OtherActor);
	// if (AsPawn && AsPawn->Controller->IsA<APlayerController>())
	// {
	// 	CompleteTask();
	// }

	if (OtherActor->IsA<AGCPlayerCharacter>())
	{
		CompleteTask();
	}
}
