// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Data/PrimitiveData.h"
#include "GameFramework/Actor.h"
#include "InventoryInspection.generated.h"

UCLASS()
class FAF_REV_API AInventoryInspection : public AActor
{
	GENERATED_BODY()

public:

	AInventoryInspection();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<class USpringArmComponent> InspectionRoot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<UStaticMeshComponent> InspectionMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<class USpotLightComponent> InspectionLight;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<USceneCaptureComponent2D> InspectionCapture;

	UPROPERTY(EditAnywhere, Category = "Settings")
		float TurnSpeed;

	void SetMeshData(const FStaticMeshProperties& InProperties, const FTransform& RelativeTransform) const;

protected:

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
};
