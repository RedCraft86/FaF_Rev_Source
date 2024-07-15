// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "CCTVCamera.h"
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
		TObjectPtr<UAudioComponent> MonitorAudio;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<UStaticMeshComponent> MonitorMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<class UWidgetComponent> MonitorWidget;

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

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "CCTV")
		void PlayMonitorAudio(USoundBase* Sound, const float Volume) const;

	UFUNCTION(BlueprintPure, Category = "CCTV")
		bool IsOnActiveCamera() const { return ActiveCamera.Value && ActiveCamera.Value->IsEnabled(); }

	UFUNCTION(BlueprintPure, Category = "CCTV")
		bool IsSeeingEnemies() const { return IsOnActiveCamera() ? ActiveCamera.Value->IsSeeingEnemies() : false; }

	UFUNCTION(BlueprintPure, Category = "CCTV")
		FText GetCameraName() const { return IsOnActiveCamera() ? ActiveCamera.Value->DisplayName : FText::GetEmpty(); }
	
protected:

	FTimerHandle ChangeCameraHandle;
	FGTInterpScalar ChangeCameraStatic;
	TPair<FName, TObjectPtr<ACCTVCamera>> ActiveCamera;

	void UpdateCameraStatic();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnEnableStateChanged(const bool bIsEnabled) override;
};
