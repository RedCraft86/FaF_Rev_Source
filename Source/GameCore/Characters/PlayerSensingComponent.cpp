// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "PlayerSensingComponent.h"
#include "Framework/GCGameInstance.h"
#include "Kismet/GameplayStatics.h"

UPlayerSensingComponent::UPlayerSensingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;

	bEnabled = true;
	bDetectOnlyMovement = false;
	MinMovementSpeed = 10.0f;
	InnerAngle = 70.0f;
	OuterAngle = 80.0f;
	MaxDistance = 5000.0f;
	SenseTime = 0.25f;
	LoseTime = 1.0f;

	PlayerChar = nullptr;
	GameInst = nullptr;
	bDetected = false;
}

void UPlayerSensingComponent::SetEnabled(const bool bInEnabled)
{
	if (bEnabled != bInEnabled)
	{
		bEnabled = bInEnabled;
		SetComponentTickEnabled(bEnabled);
	}
}

void UPlayerSensingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bool bSeen = false;
	if (bEnabled && IsValid(GameInst) ? GameInst->GetPlayerInvincible() : true)
	{
		if (bDetected)
		{
			bSeen = !(!InOuterAngle() || DistanceToPlayer() > MaxDistance || !TraceCheck());
		}
		else
		{
			bSeen = InInnerAngle() && DistanceToPlayer() <= MaxDistance && TraceCheck() && MoveCheck();
		}
	}

	if (bDetected != bSeen)
	{
		bDetected = bSeen;
		GetWorld()->GetTimerManager().ClearTimer(AnnounceTimer);
		GetWorld()->GetTimerManager().SetTimer(AnnounceTimer, this, &UPlayerSensingComponent::BroadcastEvent,
			bDetected ? SenseTime : LoseTime, false);
	}
}

bool UPlayerSensingComponent::MoveCheck() const
{
	return bDetectOnlyMovement ? IsValid(PlayerChar) ? PlayerChar->GetVelocity().Size() >= MinMovementSpeed : false : true;
}

bool UPlayerSensingComponent::TraceCheck() const
{
	if (PlayerChar)
	{
		FCollisionQueryParams Query;
		Query.AddIgnoredActor(GetOwner());
		Query.AddIgnoredActor(PlayerChar);
	
		FHitResult HitResult;
		return !GetWorld()->LineTraceSingleByChannel(HitResult, GetComponentLocation(),
			PlayerChar->GetActorLocation(), ECC_Visibility, Query);
	}

	return false;
}

bool UPlayerSensingComponent::InOuterAngle() const
{
	return AngleToPlayer() <= OuterAngle;
}

bool UPlayerSensingComponent::InInnerAngle() const
{
	return AngleToPlayer() <= InnerAngle;
}

float UPlayerSensingComponent::AngleToPlayer() const
{
	if (PlayerChar)
	{
		FVector DotA = PlayerChar->GetActorLocation() - GetComponentLocation();
		FVector DotB = GetForwardVector();

		DotA.Normalize();
		DotB.Normalize();

		return 180.0 / UE_PI * FMath::Acos(FVector::DotProduct(DotA, DotB));
	}

	return 0.0f;
}

float UPlayerSensingComponent::DistanceToPlayer() const
{
	if (PlayerChar)
	{
		return FVector::Dist(PlayerChar->GetActorLocation(), GetComponentLocation());
	}

	return 0.0f;
}

void UPlayerSensingComponent::BroadcastEvent() const
{
	if (bDetected)
	{
		OnSeen.Broadcast();	
	}
	else
	{
		OnLost.Broadcast();
	}
}

void UPlayerSensingComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(bEnabled);
	PlayerChar = UGameplayStatics::GetPlayerPawn(this, 0);
	GameInst = UGCGameInstance::Get(this);
}
