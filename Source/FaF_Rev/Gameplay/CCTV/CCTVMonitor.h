// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTActor.h"
#include "Data/MathTypes.h"
#include "CCTVMonitor.generated.h"

UCLASS(Abstract)
class FAF_REV_API ACCTVMonitor final : public AGTActor
{
	GENERATED_BODY()

	friend class ACCTVCamera;

public:

	ACCTVMonitor();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<UStaticMeshComponent> MonitorMesh;

	UPROPERTY(EditAnywhere, Category = "Settings")
		TMap<FName, TObjectPtr<ACCTVCamera>> Cameras;

	UFUNCTION(BlueprintCallable, Category = "CCTV")
		void ChangeCamera(const FName InKey);

protected:
	
	UPROPERTY(Transient) FName ActiveCamera;
	UPROPERTY(Transient) FGTInterpScalar CameraStaticAmount;
	UPROPERTY(Transient) FGTInterpScalar MonitorStaticAmount;

	ACCTVCamera* GetActiveCamera() const { return Cameras.Contains(ActiveCamera) ? Cameras[ActiveCamera] : nullptr; }

	void UpdateScreen() const;
	void UpdateCameraState();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnEnableStateChanged(const bool bIsEnabled) override;
};
