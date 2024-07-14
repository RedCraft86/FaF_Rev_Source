// Copyright (C) RedCraft86. All Rights Reserved.

#include "CCTVMonitor.h"
#include "CCTVCamera.h"

ACCTVMonitor::ACCTVMonitor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	MonitorMesh = CreateDefaultSubobject<UStaticMeshComponent>("MonitorMesh");
	MonitorMesh->SetupAttachment(SceneRoot);
}

TArray<FName> ACCTVMonitor::GetCameraOptions() const
{
	TArray<FName> Keys;
	Cameras.GenerateKeyArray(Keys);
	return Keys;
}

void ACCTVMonitor::ChangeCamera(const FName InKey)
{
	if (InKey.IsNone() || !Cameras.Contains(InKey)
		|| ActiveCamera.Key == InKey) return;

	if (ActiveCamera.Value) ActiveCamera.Value->SetEnabled(false);
	
	ChangeCameraStatic.SnapToTarget();
	MonitorMesh->SetCustomPrimitiveDataFloat(0, IsEnabled() ? ChangeCameraStatic.CurrentValue : 1.0f);

	ActiveCamera = {InKey, Cameras[InKey]};
	if (ActiveCamera.Value) ActiveCamera.Value->SetEnabled(true);
}

void ACCTVMonitor::UpdateCameraStatic()
{
	ChangeCameraStatic.TargetValue = ActiveCamera.Value && ActiveCamera.Value->IsEnabled() ? 0.0f : 1.0f;
}

void ACCTVMonitor::BeginPlay()
{
	Super::BeginPlay();
	for (const TArray<FName> CamKeys = GetCameraOptions(); const FName& Key : CamKeys)
	{
		if (ACCTVCamera* Cam = Cameras[Key])
		{
			Cam->SetMonitor(this);
		}
		else Cameras.Remove(Key);
	}

	if (!Cameras.Contains(DefaultCamera) && Cameras.Num() > 0)
	{
		DefaultCamera = GetCameraOptions()[0];
	}

	ChangeCamera(DefaultCamera);
}

void ACCTVMonitor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!ChangeCameraStatic.IsComplete())
	{
		ChangeCameraStatic.Tick(DeltaTime);
		MonitorMesh->SetCustomPrimitiveDataFloat(0, IsEnabled() ? ChangeCameraStatic.CurrentValue : 1.0f);
	}
}

void ACCTVMonitor::OnEnableStateChanged(const bool bIsEnabled)
{
	SetActorEnableCollision(bIsEnabled);
	SetActorTickEnabled(bIsEnabled);
}

