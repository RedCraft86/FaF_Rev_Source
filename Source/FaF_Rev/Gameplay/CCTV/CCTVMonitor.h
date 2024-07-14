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

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<UStaticMeshComponent> MonitorMesh;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (GetOptions = "GetCameraOptions"))
		FName DefaultCamera;

	UPROPERTY(EditAnywhere, Category = "Settings")
		TMap<FName, TObjectPtr<ACCTVCamera>> Cameras;

	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
		TObjectPtr<UTextureRenderTarget2D> CaptureRT;

	UFUNCTION(BlueprintPure, Category = "CCTV")
		TArray<FName> GetCameraOptions() const;

	UFUNCTION(BlueprintCallable, Category = "CCTV")
		void ChangeCamera(const FName InKey);

protected:

	FTimerHandle ChangeCameraHandle;
	FGTInterpScalar ChangeCameraStatic;
	TPair<FName, TObjectPtr<ACCTVCamera>> ActiveCamera;

	void UpdateCameraStatic();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnEnableStateChanged(const bool bIsEnabled) override;
};
