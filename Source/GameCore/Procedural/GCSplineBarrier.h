// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "RCPrimitiveTypes.h"
#include "Procedural/GCProceduralBase.h"
#include "GCSplineBarrier.generated.h"

UCLASS(NotBlueprintable, DisplayName = "Spline Barrier")
class GAMECORE_API AGCSplineBarrier final : public AGCProceduralBase
{
	GENERATED_BODY()

public:

	AGCSplineBarrier();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class USplineComponent* SplineComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class UProceduralMeshComponent* ProceduralMesh;

	UPROPERTY(EditAnywhere, Category = "Settings")
		bool bClosedLoop;
	
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 10.0f, UIMin = 10.0f))
		float WallHeight;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FRCPrimitiveCollision Collision;

	UPROPERTY(EditAnywhere, Category = "Settings")
		UMaterialInterface* Material;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Settings")
		UMaterialInterface* OverlayMaterial = nullptr;
#endif
	
	virtual void Construct() override;

private:
	
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
};
