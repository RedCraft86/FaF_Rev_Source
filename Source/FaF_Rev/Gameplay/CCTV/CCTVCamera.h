// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTActor.h"
#include "CCTVCamera.generated.h"

UCLASS(Abstract)
class FAF_REV_API ACCTVCamera final : public AGTActor
{
	GENERATED_BODY()

	friend class ACCTVMonitor;

public:

	ACCTVCamera();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<USceneCaptureComponent2D> CameraCapture;

protected:

	UPROPERTY(Transient) TObjectPtr<ACCTVMonitor> Monitor;
	virtual void OnEnableStateChanged(const bool bIsEnabled) override;
};
