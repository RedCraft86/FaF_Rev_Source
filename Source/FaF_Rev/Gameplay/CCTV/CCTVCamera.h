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

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<USceneComponent> CaptureView;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<USceneCaptureComponent2D> SceneCapture;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<class UVisionConeComponent> VisionCone;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FText DisplayName;
	
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		FVector2D TurningRate;
	
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		FVector2D TurningRange;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 0.01f, UIMin = 0.01f))
		float CaptureFrequency;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 0.01f, UIMin = 0.01f))
		float RefreshFrequency;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		TSet<TObjectPtr<const class AFREnemyBase>> Enemies;

	UPROPERTY(EditAnywhere, Category = "Settings")
		TSet<TObjectPtr<const AActor>> CullingActors;
	
	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
		TObjectPtr<UMaterialInterface> TrackingPostProcess;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
		FName EnemyFilterTag;
#endif
#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "Settings")
		void FindEnemies();
#endif

	UFUNCTION(BlueprintPure, Category = "CCTV")
		bool IsSeeingEnemies() const { return bSeeEnemies; }
	
protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<class UDebugShapesComponent> DebugShapes;
#endif

	bool bSeeEnemies;
	FTimerHandle CaptureHandle, RefreshHandle;
	UPROPERTY(Transient) TObjectPtr<ACCTVMonitor> Monitor;

	void CaptureScene() const;
	void RefreshEnemyState();
	void SetMonitor(ACCTVMonitor* InMonitor);
	
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void OnEnableStateChanged(const bool bIsEnabled) override;

	UFUNCTION() void OnEnemyAudioPlayed(const AFRCharacter* Enemy, const UAudioComponent* Component);
};
