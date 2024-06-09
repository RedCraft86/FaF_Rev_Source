// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GTMeshGenBase.h"
#include "SplinePolygon.generated.h"

UCLASS()
class GTRUNTIME_API ASplinePolygon : public AGTProcMeshBase
{
	GENERATED_BODY()

public:

	ASplinePolygon();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<class USplineComponent> SplineComponent;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FPrimitiveCollision Collision;

	UPROPERTY(EditAnywhere, Category = "Settings")
		TObjectPtr<UMaterialInterface> Material;

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
		TObjectPtr<UMaterialInterface> OverlayMaterial = nullptr;

	virtual void BeginPlay() override;
#endif
	virtual void Construct() override;
};
