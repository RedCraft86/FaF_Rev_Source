// Copyright (C) RedCraft86. All Rights Reserved.

#include "FREnemyBase.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "FRPlayer.h"
#include "NavigationPath.h"
#include "Libraries/GTRuntimeLibrary.h"

AFREnemyBase::AFREnemyBase() : bStartRoaming(true), EnemyState(EEnemyState::None), TargetFootstepVolume(1.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	FootstepAudio = CreateDefaultSubobject<UAudioComponent>("FootstepAudio");
	FootstepAudio->SetRelativeLocation({0.0f, 0.0f, -50.0f});
	FootstepAudio->SetupAttachment(GetCapsuleComponent());

	FootstepAudio->bOverrideAttenuation = true;
	FootstepAudio->AttenuationOverrides.bAttenuate = true;
	FootstepAudio->AttenuationOverrides.AttenuationShapeExtents = FVector::ZeroVector;
	FootstepAudio->AttenuationOverrides.FalloffDistance = 4500.0f;
}

void AFREnemyBase::PlayFootstep()
{
	if (FootstepAudio->VolumeMultiplier > 0.0f)
	{
		FootstepAudio->Play();
	}
}

void AFREnemyBase::SetEnemyState(const EEnemyState InNewState)
{
	if (EnemyState != InNewState)
	{
		EnemyState = InNewState;
		if (AFRPlayerBase* Player = PlayerChar.LoadSynchronous())
		{
			if (EnemyState == EEnemyState::None || EnemyState == EEnemyState::Roam)
			{
				Player->RemoveEnemy(this);
			}
			else
			{
				Player->AddEnemy(this);
			}
		}
	}
}

void AFREnemyBase::OnFootstepAdjust()
{
	FootstepAudio->AttenuationOverrides.bAttenuate = true;
	if (const APlayerCameraManager* CM = UGameplayStatics::GetPlayerCameraManager(this, 0))
	{
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this);
		TraceParams.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(this, 0));
		
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, CM->GetCameraLocation(),
			GetActorLocation(), ECC_Visibility, TraceParams);
		if (!HitResult.bBlockingHit)
		{
			TargetFootstepVolume = 1.0f;
			FootstepAudio->SetVolumeMultiplier(TargetFootstepVolume);
			FootstepAudio->SetRelativeLocation(FVector::ZeroVector);
			return;
		}
		
		FVector Origin, BoxExtent;
		TArray<FVector> Vertices = UGTRuntimeLibrary::GetBoundingBoxVertices(this,
			true, false, Origin, BoxExtent);
		Vertices.Add(Origin);
		for (const FVector& Vert : Vertices)
		{
			GetWorld()->LineTraceSingleByChannel(HitResult, CM->GetCameraLocation(),
				Vert, ECC_Visibility, TraceParams);
			if (!HitResult.bBlockingHit)
			{
				TargetFootstepVolume = 1.0f;
				FootstepAudio->SetVolumeMultiplier(TargetFootstepVolume);
				FootstepAudio->SetRelativeLocation(FVector::ZeroVector);
				return;
			}
		}

		const UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(this,
			GetActorLocation(), CM->GetCameraLocation(), this);
		const TArray<FVector> PathPoints = Path ? Path->PathPoints : TArray<FVector>();
		const float PathLen = Path ? Path->GetPathLength() : 0.0f;
		if (PathLen < 1000.0f || PathPoints.IsEmpty())
		{
			TargetFootstepVolume = 1.0f;
			FootstepAudio->SetVolumeMultiplier(TargetFootstepVolume);
			FootstepAudio->SetRelativeLocation(FVector::ZeroVector);
			return;
		}

		if (PathLen > FootstepAudio->AttenuationOverrides.FalloffDistance)
		{
			TargetFootstepVolume = 0.0f;
			FootstepAudio->SetVolumeMultiplier(TargetFootstepVolume);
			FootstepAudio->SetRelativeLocation(FVector::ZeroVector);
		}
		else if (PathPoints.Num() > 1)
		{
			TargetFootstepVolume = FMath::GetMappedRangeValueClamped(
				FVector2D(0.0f, FootstepAudio->AttenuationOverrides.FalloffDistance),
				FVector2D(1.0f, 0.01f), PathLen);

			FootstepAudio->SetRelativeLocation(PathPoints[PathPoints.Num() - 2] + FVector(0.0f, 0.0f, 70.0f));

#if WITH_EDITORONLY_DATA
			if (bDebugAudio)
			{
#if WITH_EDITOR
				DrawDebugSphere(GetWorld(), PathPoints[PathPoints.Num() - 1] + FVector(0.0f, 0.0f, 70.0f), 32, 16, FColor::Green,
	   false, 1.0f, 0, 1);
				for (int i = 0; i < PathPoints.Num() - 1; i++)
				{
				
					DrawDebugLine(GetWorld(), PathPoints[i] + FVector(0.0f, 0.0f, 70.0f),
						PathPoints[i + 1] + FVector(0.0f, 0.0f, 70.0f),
						FColor::Green, false, 1.0f, 0, 1);
				
					DrawDebugSphere(GetWorld(), PathPoints[i] + FVector(0.0f, 0.0f, 70.0f),
						32, 16, FColor::Green, false, 1.0f, 0, 1);
				}
#endif
			}
#endif
		}
	}
}

void AFREnemyBase::BeginPlay()
{
	Super::BeginPlay();
	SetEnemyState(bStartRoaming ? EEnemyState::Roam : EEnemyState::None);
	GetWorldTimerManager().SetTimer(FootstepAdjustTimer, this, &AFREnemyBase::OnFootstepAdjust, 0.5f, true);
}

void AFREnemyBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

#if WITH_EDITORONLY_DATA
	if (bDebugAudio)
	{
#if WITH_EDITOR
		UKismetSystemLibrary::PrintString(this, FString::SanitizeFloat(FootstepAudio->VolumeMultiplier) + TEXT("\n")
			+ FootstepAudio->GetComponentLocation().ToString(), true, false, FColor::Yellow, 1.0f, GetFName());
#endif
	}
#endif
	
	FootstepAudio->SetVolumeMultiplier(FMath::FInterpConstantTo(FootstepAudio->VolumeMultiplier,
		TargetFootstepVolume, DeltaSeconds, 0.1f));
}

void AFREnemyBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (PlayerChar.IsNull()) PlayerChar = AFRPlayerBase::Get(this);
}

EEnemyState AFREnemyBase::GetHighestEnemyState(const TArray<AFREnemyBase*>& InEnemies)
{
	for (const AFREnemyBase* Enemy : InEnemies)
	{
		if (!Enemy) continue;
		switch (Enemy->GetEnemyState())
		{
		case EEnemyState::Chase: return EEnemyState::Chase;
		case EEnemyState::Search: return EEnemyState::Search;
		case EEnemyState::Alert: return EEnemyState::Alert;
		case EEnemyState::Roam: return EEnemyState::Roam;
		case EEnemyState::None: return EEnemyState::None;
		}
	}

	return EEnemyState::None;
}
