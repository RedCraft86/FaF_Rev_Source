// Copyright (C) RedCraft86. All Rights Reserved.

#include "CCTVMonitor.h"
#include "CCTVCamera.h"
#include "CCTVWidget.h"

ACCTVMonitor::ACCTVMonitor()
{
	PrimaryActorTick.bCanEverTick = true;

	MonitorMesh = CreateDefaultSubobject<UStaticMeshComponent>("MonitorMesh");
	MonitorMesh->SetupAttachment(SceneRoot);

	ScreenWidget = CreateDefaultSubobject<UWidgetComponent>("ScreenWidget");
	ScreenWidget->SetupAttachment(MonitorMesh);
	
	CameraStaticAmount.InterpSpeed = 5.0f;
	MonitorStaticAmount.InterpSpeed = 5.0f;
}

void ACCTVMonitor::ChangeCamera(const FName InKey)
{
	if (bSwapping) return;
	if (!InKey.IsNone() && ActiveCameraID != InKey && Cameras.Contains(InKey))
	{
		if (ACCTVCamera* OldCam = ActiveCameraPtr)
		{
			OldCam->SetEnabled(false);
		}
		CameraStaticAmount.SnapToTarget();
		UpdateScreen();

		bSwapping = true;
		ActiveCameraID = InKey;
		ActiveCameraPtr = Cameras[ActiveCameraID];
		
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, [this]()
		{
			bSwapping = false;
			if (ACCTVCamera* NewCam = ActiveCameraPtr)
			{
				NewCam->SetEnabled(true);
			}
		}, 0.5f, false);
	}
}

void ACCTVMonitor::UpdateCameraState()
{
	CameraStaticAmount.TargetValue = ActiveCameraPtr ? (ActiveCameraPtr->IsEnabled() ? 0.0f : 1.0f) : 1.0f;
	if (UCCTVWidget* Widget = Cast<UCCTVWidget>(ScreenWidget->GetWidget()))
	{
		Widget->ActiveCamera = ActiveCameraPtr;
		Widget->UpdateData();
	}
}

void ACCTVMonitor::UpdateScreen() const
{
	MonitorMesh->SetCustomPrimitiveDataFloat(0, ActiveCameraPtr ?
		FMath::Max(1.0f, CameraStaticAmount.CurrentValue + MonitorStaticAmount.CurrentValue) : 1.0f);
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
	ScreenWidget->SetHiddenInGame(!bIsEnabled);
}
