// Copyright (C) RedCraft86. All Rights Reserved.

#include "CCTVCamera.h"
#include "CCTVMonitor.h"
#include "EnemyAI/FREnemyBase.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Libraries/GTRuntimeLibrary.h"

ACCTVCamera::ACCTVCamera() : ViewAngle(60.0f), bCapturePending(false)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickInterval = 0.05f;

	CameraCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("CameraCapture");
	CameraCapture->SetupAttachment(SceneRoot);
	CameraCapture->bAlwaysPersistRenderingState = true;
	CameraCapture->bCaptureEveryFrame = false;
	
	bEnabled = false;
}

void ACCTVCamera::AddYawRotation()
{
	CurrentOffset.X = FMath::Clamp(CurrentOffset.X + TurnRate.X, -FMath::Abs(TurnRange.X), FMath::Abs(TurnRange.X));
}

void ACCTVCamera::AddPitchRotation()
{
	CurrentOffset.Y = FMath::Clamp(CurrentOffset.Y + TurnRate.Y, -FMath::Abs(TurnRange.Y), FMath::Abs(TurnRange.Y));
}

void ACCTVCamera::TrackEnemies()
{
	if (!IsEnabled())
	{
		bSeeEnemies = false;
		CameraCapture->AddOrUpdateBlendable(TrackerPostProcess, 0.0f);
		return;
	}
	
	const FVector Forward = CameraCapture->GetForwardVector();
	const FVector Position = CameraCapture->GetComponentLocation();
	for (const TObjectPtr<AFREnemyBase>& Enemy : Enemies)
	{
		if (!Enemy) return;
		FVector Origin, BoxExtent;
		TArray<FVector> Verts = UGTRuntimeLibrary::GetBoundingBoxVertices(Enemy,
			false, false, Origin, BoxExtent);
		Verts.Add(Origin);

		for (const FVector& Vert : Verts)
		{
			FCollisionQueryParams Params; Params.AddIgnoredActor(this); Params.AddIgnoredActor(Enemy);
			if (FHitResult HitResult; GetWorld()->LineTraceSingleByChannel(HitResult,
				Position, Vert, ECC_Visibility, Params))
			{
				continue;
			}
			
			if (180.0 / UE_DOUBLE_PI * FMath::Acos(FVector::DotProduct(Forward, (Vert - Position).GetSafeNormal())) <= ViewAngle)
			{
				bSeeEnemies = true;
				CameraCapture->AddOrUpdateBlendable(TrackerPostProcess, 1.0f);
				return;
			}
		}
	}

	bSeeEnemies = false;
	CameraCapture->AddOrUpdateBlendable(TrackerPostProcess, 0.0f);
}

void ACCTVCamera::BeginPlay()
{
	Super::BeginPlay();
	CameraCapture->TextureTarget = TextureRenderTarget;
	BaseRotation = CameraCapture->GetRelativeRotation();
	GetWorldTimerManager().SetTimer(TrackerTimer, this, &ACCTVCamera::TrackEnemies, 0.1f, true);
}

void ACCTVCamera::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CameraCapture->SetRelativeRotation({
		CurrentOffset.Y + BaseRotation.Pitch,
		CurrentOffset.X + BaseRotation.Yaw,
		BaseRotation.Roll
	});

	if (!bCapturePending)
	{
		bCapturePending = true;
		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this]()
		{
			CameraCapture->CaptureScene();
			AsyncTask(ENamedThreads::GameThread, [this]()
			{
				bCapturePending = false;	
			});
		});
	}
}

void ACCTVCamera::OnEnableStateChanged(const bool bIsEnabled)
{
	if (!Monitor) return;
	if (Monitor->ActiveCameraPtr == this)
	{
		Monitor->UpdateCameraState();
		SetActorTickEnabled(PrimaryActorTick.bStartWithTickEnabled && bIsEnabled);	
	}

	bIsEnabled ? GetWorldTimerManager().UnPauseTimer(TrackerTimer) : GetWorldTimerManager().UnPauseTimer(TrackerTimer);
	TrackEnemies();
}
