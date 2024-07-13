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

	UPROPERTY(EditAnywhere, Category = "Settings")
		FText DisplayName;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		FVector2D TurnRate;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		FVector2D TurnRange;

	UPROPERTY(EditAnywhere, Category = "Settings")
		float ViewAngle;

	UPROPERTY(EditAnywhere, Category = "Settings")
		TObjectPtr<UMaterialInterface> TrackerPostProcess;

	UPROPERTY(EditAnywhere, Category = "Settings")
		TObjectPtr<UTextureRenderTarget2D> TextureRenderTarget;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		TSet<TObjectPtr<class AFREnemyBase>> Enemies;

	UFUNCTION(BlueprintCallable, Category = "CCTV")
		void AddYawRotation();

	UFUNCTION(BlueprintCallable, Category = "CCTV")
		void AddPitchRotation();

	UFUNCTION(BlueprintPure, Category = "CCTV")
		bool SeesEnemies() const { return bSeeEnemies; }

protected:

	UPROPERTY() bool bSeeEnemies;
	UPROPERTY() bool bCapturePending;
	UPROPERTY() FRotator BaseRotation;
	UPROPERTY() FVector2D CurrentOffset;
	UPROPERTY() FTimerHandle TrackerTimer;
	UPROPERTY(Transient) TObjectPtr<ACCTVMonitor> Monitor;

	void TrackEnemies();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnEnableStateChanged(const bool bIsEnabled) override;
};
