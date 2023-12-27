// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCBoxHoldInteract.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#if WITH_EDITOR
#include "Components/BillboardComponent.h"
#endif

AGCBoxHoldInteract::AGCBoxHoldInteract()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

#if WITH_EDITORONLY_DATA
	CollisionBox->ShapeColor = FColor(160, 160, 250, 255);
#endif
#if WITH_EDITOR
	if (VisualIcon)
	{
		ConstructorHelpers::FObjectFinder<UTexture2D> IconFinder(TEXT("/Engine/EditorResources/Spawn_Point.Spawn_Point"));
		if (IconFinder.Succeeded())
		{
			VisualIcon->Sprite = IconFinder.Object;
		}
	}
#endif

	FillDuration = 2.0f;
	bShowingWidget = false;
	bBeingInteracted = false;
	CompletePercent = 0.0f;
}

void AGCBoxHoldInteract::LockTask()
{
	Super::LockTask();
	CompletePercent = 0.0f;
}

void AGCBoxHoldInteract::ResetTask()
{
	Super::ResetTask();
	CompletePercent = 0.0f;
}

void AGCBoxHoldInteract::OnBeginInteract_Implementation(AGCPlayerCharacter* Player, const FHitResult& HitResult)
{
	bBeingInteracted = true;
	SetWidgetState(true);
}

void AGCBoxHoldInteract::OnEndInteract_Implementation(AGCPlayerCharacter* Player)
{
	bBeingInteracted = false;
	SetWidgetState(false);
}

void AGCBoxHoldInteract::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bCompleted && bBeingInteracted)
	{
		CompletePercent = FMath::FInterpConstantTo(CompletePercent, 1.0f,
			DeltaSeconds, 1.0f / FMath::Max(0.1f, FillDuration));

		if (CompletePercent >= 1.0f)
		{
			CompleteTask();
		}
	}
}

void AGCBoxHoldInteract::SetWidgetState(const bool bNewState)
{
	if (bShowingWidget != bNewState)
	{
		bShowingWidget = bNewState;
		ShouldShowWidget(bShowingWidget);
	}
}

AGCBoxHoldInteractRanged::AGCBoxHoldInteractRanged()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	RangeBox = CreateDefaultSubobject<UBoxComponent>("RangeBox");
	RangeBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RangeBox->SetCollisionObjectType(ECC_WorldDynamic);
	RangeBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	RangeBox->SetupAttachment(SceneRoot);

	RangeSphere = CreateDefaultSubobject<USphereComponent>("RangeSphere");
	RangeSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RangeSphere->SetCollisionObjectType(ECC_WorldDynamic);
	RangeSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	RangeSphere->SetupAttachment(SceneRoot);

	ShapeType = TEXT("Box");
	IconLocation = {0.0f, 0.0f, 10.0f};
	ShapeOrigin = FVector::ZeroVector;
	MaxExtent = FVector(150.0f);
	ZCache = 150.0f;
}

bool AGCBoxHoldInteractRanged::IsPlayerInRange() const
{
	if (ShapeType.IsNone()) return false;

	const APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
	switch (Shapes.Find(ShapeType))
	{
	case 0:
		{
			return RangeBox->Bounds.GetBox().IsInside(Pawn->GetActorLocation());
		}

	case 1:
		{
			const float Dist = FVector::Dist2D(RangeSphere->GetComponentLocation(), Pawn->GetActorLocation());
			return Dist < RangeSphere->GetUnscaledSphereRadius();
		}
			
	default: return false;
	}
}

void AGCBoxHoldInteractRanged::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!bCompleted && CompletePercent > 0.0f)
	{
		if (TickDelta > 0.25f)
		{
			TickDelta = 0.0f;
			SetWidgetState(IsPlayerInRange());
			if (!bShowingWidget)
			{
				CompletePercent = 0.0f;
			}
		}
		else
		{
			TickDelta += DeltaSeconds;
		}
	}
}

void AGCBoxHoldInteractRanged::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (!ShapeType.IsNone())
	{
		switch (Shapes.Find(ShapeType))
		{
		case 0:
			if (FMath::IsNearlyZero(MaxExtent.Z))
			{
				MaxExtent.Z = ZCache;
			}
			RangeSphere->SetVisibility(false);
			RangeBox->SetVisibility(true);
			RangeBox->SetRelativeLocation(ShapeOrigin);
			RangeBox->SetBoxExtent({
				FMath::Max(FMath::Abs(MaxExtent.X), 4.0f),
				FMath::Max(FMath::Abs(MaxExtent.Y), 4.0f),
				FMath::Max(FMath::Abs(MaxExtent.Z), 4.0f)
			});
			break;

		case 1:
			if (MaxExtent.Z > 0.0f)
			{
				ZCache = MaxExtent.Z;
				MaxExtent.Z = 0.0f;
			}
			RangeBox->SetVisibility(false);
			RangeSphere->SetVisibility(true);
			RangeSphere->SetRelativeLocation(ShapeOrigin);
			RangeSphere->SetSphereRadius(MaxExtent.Size2D());
			break;
			
		default: break;
		}
	}
}

void AGCBoxHoldInteractRanged::OnBeginInteract_Implementation(AGCPlayerCharacter* Player, const FHitResult& HitResult)
{
	bBeingInteracted = true;
}

void AGCBoxHoldInteractRanged::OnEndInteract_Implementation(AGCPlayerCharacter* Player)
{
	bBeingInteracted = false;
}
