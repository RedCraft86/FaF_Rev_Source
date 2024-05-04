// Copyright (C) RedCraft86. All Rights Reserved.

#include "PlayerTeleporter.h"
#include "Components/CapsuleComponent.h"
#include "GameplayTagContainer.h"
#include "EngineUtils.h"
#include "FRPlayer.h"

APlayerTeleporter::APlayerTeleporter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void APlayerTeleporter::TeleportPlayer() const
{
	if (AFRPlayerBase* Player = PlayerReference.LoadSynchronous())
	{
		Player->TeleportPlayer(GetActorLocation(), GetActorRotation());
	}
}

void APlayerTeleporter::BeginPlay()
{
	Super::BeginPlay();
	if (PlayerReference.IsNull()) PlayerReference = FRPlayer(this);
	if (const AFRPlayerBase* Player = PlayerReference.LoadSynchronous())
	{
		FHitResult Result;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(Player);
		if (GetWorld()->LineTraceSingleByChannel(Result, GetActorLocation(),
			GetActorLocation() - FVector(0.0f, 0.0f, 100.0f), ECC_Visibility, Params))
		{
			SetActorLocation(Result.Location + FVector(0.0f, 0.0f,
				Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
		}
	}
}

APlayerTeleporter* APlayerTeleporter::FindTeleporter(const UObject* WorldContextObject, const FGameplayTag InFilter)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	for (APlayerTeleporter* Found : TActorRange<APlayerTeleporter>(World))
	{
		if (IsValid(Found) && Found->TeleporterIDs.HasTagExact(InFilter)) return Found;
	}
		
	return nullptr;
}
