// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTActor.h"
#include "Data/MathTypes.h"
#include "Components/WidgetComponent.h"
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<UWidgetComponent> ScreenWidget;

	UPROPERTY(EditAnywhere, Category = "Settings")
		TMap<FName, TObjectPtr<ACCTVCamera>> Cameras;

	UFUNCTION(BlueprintCallable, Category = "CCTV")
		void ChangeCamera(const FName InKey);

protected:

	UPROPERTY() bool bSwapping;
	UPROPERTY() FName ActiveCameraID;
	UPROPERTY() FGTInterpScalar CameraStaticAmount;
	UPROPERTY() FGTInterpScalar MonitorStaticAmount;
	UPROPERTY(Transient) TObjectPtr<ACCTVCamera> ActiveCameraPtr;

	void UpdateCameraState();
	void UpdateScreen() const;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnEnableStateChanged(const bool bIsEnabled) override;
};
