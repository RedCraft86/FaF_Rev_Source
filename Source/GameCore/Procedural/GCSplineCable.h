// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "RCPrimitiveTypes.h"
#include "Procedural/GCProceduralBase.h"
#include "Components/SplineMeshComponent.h"
#include "GCSplineCable.generated.h"

UCLASS(NotBlueprintable, DisplayName = "Spline Cable")
class GAMECORE_API AGCSplineCable final : public AGCProceduralBase
{
	GENERATED_BODY()

public:

	AGCSplineCable();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class USplineComponent* SplineComponent;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		FRCPrimitiveCollision Collision;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		FGCSplineStaticMesh CableMesh;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FGCProceduralStaticMesh StartCap;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FGCProceduralStaticMesh EndCap;

	UPROPERTY(EditAnywhere, Category = "Settings|Attachments")
		uint8 AttachmentAmount;
	
	UPROPERTY(EditAnywhere, Category = "Settings|Attachments")
		EGCProceduralSpawnType AttachmentSpawnType;

	UPROPERTY(EditAnywhere, Category = "Settings|Attachments")
		FTransform AttachTransform;
	
	UPROPERTY(EditAnywhere, Category = "Settings|Attachments")
		TArray<FGCProceduralStaticMesh> AttachOptions;

	virtual void Construct() override;

private:

	UPROPERTY(Transient)
		TArray<USplineMeshComponent*> SplineMeshes;

	UPROPERTY(Transient)
		TArray<UStaticMeshComponent*> AttachedMeshes;
	
	virtual void OnConstruction(const FTransform& Transform) override;
};
