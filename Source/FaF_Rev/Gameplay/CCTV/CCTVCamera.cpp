// Copyright (C) RedCraft86. All Rights Reserved.

#include "CCTVCamera.h"
#include "CCTVMonitor.h"
#include "Components/SceneCaptureComponent2D.h"

ACCTVCamera::ACCTVCamera()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	CameraCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("CameraCapture");
	CameraCapture->SetupAttachment(SceneRoot);
	CameraCapture->bAlwaysPersistRenderingState = true;
	
	bEnabled = false;
}

void ACCTVCamera::OnEnableStateChanged(const bool bIsEnabled)
{
	if (!Monitor) return;
	if (Monitor->GetActiveCamera() == this)
	{
		Monitor->UpdateCameraState();
		CameraCapture->bCaptureEveryFrame = bIsEnabled;
		SetActorTickEnabled(PrimaryActorTick.bStartWithTickEnabled && bIsEnabled);	
	}
}
