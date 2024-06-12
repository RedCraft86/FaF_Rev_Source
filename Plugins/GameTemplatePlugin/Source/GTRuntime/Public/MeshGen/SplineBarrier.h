// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTMeshGenBase.h"
#include "SplineBarrier.generated.h"

UCLASS(NotBlueprintable, DisplayName = "Spline Barrier")
class GTRUNTIME_API ASplineBarrier final : public AGTMeshGenBase
{
	GENERATED_BODY()

public:

	ASplineBarrier();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<class USplineComponent> SplineComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<class UHierarchicalInstancedStaticMeshComponent> WallMeshComponent;

	UPROPERTY(EditAnywhere, Category = "Settings")
		bool bClosedLoop;
	
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 0.5f, UIMin = 0.5f))
		float WallHeight;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 0, UIMin = 0))
		TSet<int32> SkipIndexes;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FPrimitiveCollision Collision;
	
	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
		TObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
		TObjectPtr<UMaterialInterface> Material;

private:
#if WITH_EDITOR
	void LoadObjects();
#endif
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
		TObjectPtr<UMaterialInterface> OverlayMaterial = nullptr;
#endif
	
	virtual void BeginPlay() override;
	virtual void Construct() override;
};
