// Copyright (C) RedCraft86. All Rights Reserved.

#include "FREnemyBase.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Libraries/GTRuntimeLibrary.h"
#include "FRPlayer.h"

AFREnemyBase::AFREnemyBase() : bStartRoaming(true), EnemyState(EEnemyState::None)
{
	PrimaryActorTick.bCanEverTick = true;

	FootstepAudio = CreateDefaultSubobject<UAudioComponent>("FootstepAudio");
	FootstepAudio->SetRelativeLocation({0.0f, 0.0f, -50.0f});
	FootstepAudio->SetupAttachment(GetCapsuleComponent());
	FootstepAudio->bEnableLowPassFilter = true;
	FootstepAudio->bOverrideAttenuation = true;
	FootstepAudio->AttenuationOverrides.bAttenuate = true;
	FootstepAudio->AttenuationOverrides.AttenuationShapeExtents = FVector::ZeroVector;
	FootstepAudio->AttenuationOverrides.FalloffDistance = 4500.0f;

	AudioVolumeCurve.GetRichCurve()->UpdateOrAddKey(0.0f, 1.0f);
	AudioVolumeCurve.GetRichCurve()->UpdateOrAddKey(1.0f, 0.01f);
}

void AFREnemyBase::PlayFootstep()
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
			FootstepAudio->SetVolumeMultiplier(1.0f);
			FootstepAudio->SetRelativeLocation(FVector::ZeroVector);
			FootstepAudio->LowPassFilterFrequency = MAX_FILTER_FREQUENCY;
			FootstepAudio->Play();
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
				FootstepAudio->SetVolumeMultiplier(1.0f);
				FootstepAudio->SetRelativeLocation(FVector::ZeroVector);
				FootstepAudio->LowPassFilterFrequency = MAX_FILTER_FREQUENCY;
				FootstepAudio->Play();
				return;
			}
		}

		const UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(this,
			GetActorLocation(), CM->GetCameraLocation(), this);
		const TArray<FVector> PathPoints = Path ? Path->PathPoints : TArray<FVector>();
		const float PathLen = Path ? Path->GetPathLength() : 0.0f;
		if (PathLen < 1000.0f || PathPoints.IsEmpty())
		{
			FootstepAudio->SetVolumeMultiplier(1.0f);
			FootstepAudio->SetRelativeLocation(FVector::ZeroVector);
			FootstepAudio->LowPassFilterFrequency = MAX_FILTER_FREQUENCY;
			FootstepAudio->Play();
			return;
		}

		if (PathLen > FootstepAudio->AttenuationOverrides.FalloffDistance)
		{
			FootstepAudio->SetVolumeMultiplier(0.0f);
			FootstepAudio->SetRelativeLocation(FVector::ZeroVector);
			FootstepAudio->LowPassFilterFrequency = MAX_FILTER_FREQUENCY;
		}
		else if (PathPoints.Num() > 1)
		{
			FootstepAudio->AttenuationOverrides.bAttenuate = false;
			
			FVector2D CurveRange; AudioVolumeCurve.GetTimeRange(CurveRange.X, CurveRange.Y);
			FootstepAudio->SetVolumeMultiplier(AudioVolumeCurve.GetValue(FMath::GetMappedRangeValueClamped(
				FVector2D(0.0f, FootstepAudio->AttenuationOverrides.FalloffDistance),
				CurveRange, PathLen)));

			FootstepAudio->LowPassFilterFrequency = MAX_FILTER_FREQUENCY * 0.5f;
			FootstepAudio->SetWorldLocation(PathPoints[PathPoints.Num() - 2] + FVector(0.0f, 0.0f, 70.0f));
			FootstepAudio->Play();

#if WITH_EDITORONLY_DATA
			if (bDebugAudio)
			{
#if WITH_EDITOR
				UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("[%s] Vol: %f | Pos: %s"),
					*GetName(), FootstepAudio->VolumeMultiplier, *FootstepAudio->GetComponentLocation().ToString()),
					true, false, DebugColor, 1.0f, GetFName());
				
				DrawDebugSphere(GetWorld(), PathPoints[PathPoints.Num() - 1] + FVector(0.0f, 0.0f, 70.0f),
					8, 16, DebugColor, false, 1.0f, 0, 1);
				
				for (int i = 0; i < PathPoints.Num() - 1; i++)
				{
				
					DrawDebugLine(GetWorld(), PathPoints[i] + FVector(0.0f, 0.0f, 70.0f),
						PathPoints[i + 1] + FVector(0.0f, 0.0f, 70.0f),
						DebugColor, false, 1.0f, 0, 1);
				
					DrawDebugSphere(GetWorld(), PathPoints[i] + FVector(0.0f, 0.0f, 70.0f),
						8, 16, DebugColor, false, 1.0f, 0, 1);
				}
#endif
			}
#endif
		}
	}
	else
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

void AFREnemyBase::BeginPlay()
{
	Super::BeginPlay();
	SetEnemyState(bStartRoaming ? EEnemyState::Roam : EEnemyState::None);
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
