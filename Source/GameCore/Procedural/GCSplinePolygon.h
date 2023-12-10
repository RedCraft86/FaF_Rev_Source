// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "RCPrimitiveTypes.h"
#include "Procedural/GCProceduralBase.h"
#include "GCSplinePolygon.generated.h"

UCLASS(NotBlueprintable, DisplayName = "Spline Polygon")
class GAMECORE_API AGCSplinePolygon final : public AGCProceduralBase
{
	GENERATED_BODY()

public:

	AGCSplinePolygon();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class USplineComponent* SplineComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class UProceduralMeshComponent* ProceduralMesh;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		FRCPrimitiveCollision Collision;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Settings")
		UMaterialInterface* Material = nullptr;

	UPROPERTY(EditAnywhere, Category = "Settings")
		UMaterialInterface* OverlayMaterial = nullptr;
#endif
	
	virtual void Construct() override;

private:
#if UE_BUILD_SHIPPING
	virtual void BeginPlay() override;
#endif
	
	virtual void OnConstruction(const FTransform& Transform) override;
};
