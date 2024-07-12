// Copyright (C) RedCraft86. All Rights Reserved.

#include "CCTVMonitor.h"
#include "CCTVCamera.h"

ACCTVMonitor::ACCTVMonitor()
{
	PrimaryActorTick.bCanEverTick = true;

	MonitorMesh = CreateDefaultSubobject<UStaticMeshComponent>("MonitorMesh");
	MonitorMesh->SetupAttachment(SceneRoot);
	
	CameraStaticAmount.InterpSpeed = 5.0f;
	MonitorStaticAmount.InterpSpeed = 5.0f;
}

void ACCTVMonitor::ChangeCamera(const FName InKey)
{
	if (!InKey.IsNone() && ActiveCamera != InKey && Cameras.Contains(InKey))
	{
		if (ACCTVCamera* OldCam = GetActiveCamera())
		{
			OldCam->SetEnabled(false);
		}
		CameraStaticAmount.SnapToTarget();
		UpdateScreen();

		ActiveCamera = InKey;
		if (ACCTVCamera* NewCam = GetActiveCamera())
		{
			NewCam->SetEnabled(true);
		}
	}
}

void ACCTVMonitor::UpdateScreen() const
{
	MonitorMesh->SetCustomPrimitiveDataFloat(0, FMath::Max(1.0f,
		CameraStaticAmount.CurrentValue + MonitorStaticAmount.CurrentValue));
}

void ACCTVMonitor::UpdateCameraState()
{
	const ACCTVCamera* Cam = GetActiveCamera();
	CameraStaticAmount.TargetValue = Cam ? (Cam->IsEnabled() ? 0.0f : 1.0f) : 1.0f;
}

void ACCTVMonitor::BeginPlay()
{
	Super::BeginPlay();
	TSet<FName> Removals = {NAME_None};
	for (const TPair<FName, TObjectPtr<ACCTVCamera>>& Pair : Cameras) if (!Pair.Value) Removals.Add(Pair.Key);
	for (const FName& Name : Removals) Cameras.Remove(Name);
}

void ACCTVMonitor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!CameraStaticAmount.IsComplete() || !MonitorStaticAmount.IsComplete())
	{
		CameraStaticAmount.Tick(DeltaTime);
		MonitorStaticAmount.Tick(DeltaTime);
		UpdateScreen();
	}
}

void ACCTVMonitor::OnEnableStateChanged(const bool bIsEnabled)
{
	SetActorEnableCollision(bIsEnabled);
	SetActorTickEnabled(PrimaryActorTick.bStartWithTickEnabled && bIsEnabled);
	MonitorStaticAmount.TargetValue = bEnabled ? 0.0f : 1.0f;
}
