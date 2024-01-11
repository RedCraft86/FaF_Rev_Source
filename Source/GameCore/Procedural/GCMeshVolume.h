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

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		UInstancedStaticMeshComponent* InstancedMeshes;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 1, UIMin = 1))
		int32 MeshCount;

	UPROPERTY(EditAnywhere, Category = "Settings")
	FTransform MeshTransform;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		FRCStaticMeshProperties MeshData;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		FRCPrimitiveCollision Collision;

	virtual void Construct() override;
	
private:

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
