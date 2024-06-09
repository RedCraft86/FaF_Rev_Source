// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTMeshGenBase.h"
#include "SplineWireGen.generated.h"

UCLASS(DisplayName = "Spline Wire Generator")
class GTRUNTIME_API ASplineWireGen final : public AGTMeshGenBase
{
	GENERATED_BODY()

public:

	ASplineWireGen();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<class USplineComponent> SplineComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TSet<TObjectPtr<USplineMeshComponent>> SplineMeshComponents;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TArray<TObjectPtr<UInstancedStaticMeshComponent>> AttachMeshComponents;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<UStaticMeshComponent> StartMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<UStaticMeshComponent> EndMeshComponent;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FPrimitiveCollision Collision;
	
	UPROPERTY(EditAnywhere, Category = "Settings|Cable")
		FSplineMeshData CableMesh;

	UPROPERTY(EditAnywhere, Category = "Settings|Cable")
		FTransformMeshData StartCap;

	UPROPERTY(EditAnywhere, Category = "Settings|Cable")
		FTransformMeshData EndCap;

	UPROPERTY(EditAnywhere, Category = "Settings|Attachments")
		uint8 AttachmentCount;

	UPROPERTY(EditAnywhere, Category = "Settings|Attachments")
		EGeneratorLoopMode AttachmentLoopMode;
	
	UPROPERTY(EditAnywhere, Category = "Settings|Attachments")
		TArray<FTransformMeshData> AttachmentSample;

protected:

	TArray<int32> AttachmentIndexes;
	virtual void Construct() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
