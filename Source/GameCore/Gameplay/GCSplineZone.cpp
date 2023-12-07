// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "GCSplineZone.h"
#include "Components/SplineComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"

AGCSplineZone::AGCSplineZone()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

	SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	SplineComponent->AddSplinePoint(FVector(50.0f, 50.0f, 0.0f), ESplineCoordinateSpace::Local);
	SplineComponent->SetupAttachment(GetRootComponent());

	BoundingSphere = CreateDefaultSubobject<USphereComponent>("BoundingSphere");
	BoundingSphere->SetupAttachment(GetRootComponent());

#if WITH_EDITORONLY_DATA
	SceneRoot->bVisualizeComponent = true;
	bRunConstructionScriptOnDrag = true;
#endif
	
	bFadeTransition = false;
	bDoubledRadius = false;
	TransitionRadius = 5.0f;

	PlayerChar = nullptr;
	bPlayerInZone = false;
	PlayerTransition = 0.0f;
}

void AGCSplineZone::UpdateZone(const bool bIsInZone)
{
	if (bPlayerInZone != bIsInZone)
	{
		bPlayerInZone = bIsInZone;
		if (bPlayerInZone)
		{
			OnEnteredZone.Broadcast();
			OnEnteredZoneBP.Broadcast();
		}
		else
		{
			OnLeftZone.Broadcast();
			OnLeftZoneBP.Broadcast();
		}
	}
}

void AGCSplineZone::UpdateFade(const float TransitionValue)
{
	if (PlayerTransition != TransitionValue)
	{
		PlayerTransition = TransitionValue;
		if (bFadeTransition)
		{
			OnTransition.Broadcast(PlayerTransition);
			OnTransitionBP.Broadcast(PlayerTransition);
		}
	}
}

void AGCSplineZone::BeginPlay()
{
	Super::BeginPlay();
}

void AGCSplineZone::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (PlayerChar)
	{
		FVector PlayerLoc = PlayerChar->GetActorLocation();
		PlayerLoc.Z = GetActorLocation().Z;
		
		const FVector DistToPlayer = SplineComponent->FindLocationClosestToWorldLocation(PlayerLoc, ESplineCoordinateSpace::World) - PlayerLoc;
		const FVector2D DotA = FVector2D(SplineComponent->FindRightVectorClosestToWorldLocation(PlayerLoc, ESplineCoordinateSpace::World));
		const FVector2D DotB = FVector2D(DistToPlayer).GetSafeNormal();

		const float Direction = FVector2D::DotProduct(DotA, DotB) < 0 ? 1.0f : -1.0f;
		if (bFadeTransition)
		{
			UpdateFade(FMath::GetMappedRangeValueClamped(
				FVector2D(bDoubledRadius ? TransitionRadius * -1.0f : 0, TransitionRadius),
				FVector2D(0.0f, 1.0f), Direction * DistToPlayer.Size()));
		}
		else
		{
			UpdateFade(FMath::Clamp(Direction * DistToPlayer.Size(), 0.0f, 1.0f));
		}

		UpdateZone(FMath::IsNearlyEqual(PlayerTransition, 1.0f));
	}
	else
	{
		UpdateZone(false);
		UpdateFade(0.0f);
	}
}

void AGCSplineZone::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
#if WITH_EDITOR
	SplineComponent->bShouldVisualizeScale = bFadeTransition;
	SplineComponent->ScaleVisualizationWidth = TransitionRadius;
#endif
	SplineComponent->SetClosedLoop(true);
	SplineComponent->SetRelativeLocation(FVector::ZeroVector);
	
	const int32 NumOfSplinePoints = SplineComponent->GetNumberOfSplinePoints();
	TArray<FVector> SplinePoints = {};
	SplinePoints.Reserve(NumOfSplinePoints);
	for (int32 i = 0; i < NumOfSplinePoints; i++)
	{
		const FVector Loc = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		SplineComponent->SetLocationAtSplinePoint(i, { Loc.X, Loc.Y, 0.0f }, ESplineCoordinateSpace::Local);
		SplinePoints.Add(Loc);
	}

	const FVector CenterPoint = UKismetMathLibrary::GetVectorArrayAverage(SplinePoints);
	BoundingSphere->SetRelativeLocation(CenterPoint);

	float BaseRadius = 0.0f;
	const int32 NumOfPoints = NumOfSplinePoints * 4;
	for (int32 i = 0; i < NumOfPoints; i ++)
	{
		const float Dist = i * SplineComponent->GetSplineLength() / NumOfPoints;
		const FVector Loc = SplineComponent->GetLocationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::Local);
		BaseRadius = FMath::Max(FVector::Dist(CenterPoint, Loc), BaseRadius);
	}

	BoundingSphere->SetSphereRadius(BaseRadius + TransitionRadius + 50.0f);
}

void AGCSplineZone::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (OtherActor == GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		PlayerChar = OtherActor;
		SetActorTickEnabled(true);
	}
}

void AGCSplineZone::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	if (OtherActor == GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		PlayerChar = nullptr;
		SetActorTickEnabled(false);
	}
}
