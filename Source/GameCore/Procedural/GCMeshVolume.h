// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "GCProceduralBase.h"
#include "GCMeshVolume.generated.h"

UCLASS(NotBlueprintable, DisplayName = "Mesh Scatter Volume")
class GAMECORE_API AGCMeshVolume final : public AGCProceduralBase
{
	GENERATED_BODY()

public:
	
	AGCMeshVolume();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class UBoxComponent* BoundingBox;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 1, UIMin = 1))
		int32 MeshCount;

	UPROPERTY(EditAnywhere, Category = "Settings")
	FTransform MeshTransform;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		FRCStaticMeshProperties MeshData;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		FRCPrimitiveCollision Collision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (MakeEditWidget = true))
		FVector BoxExtent;

	virtual void Construct() override;
	
private:
	
	UPROPERTY(Transient)
		TArray<UStaticMeshComponent*> StaticMeshes;
	
#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override
	{
		if (MeshData.Mesh)
		{
			MeshData.FillMaterials();
		}
		Super::OnConstruction(Transform);
	}
#endif
};
