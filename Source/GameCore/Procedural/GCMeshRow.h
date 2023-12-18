// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "GCProceduralBase.h"
#include "GCMeshRow.generated.h"

UCLASS(NotBlueprintable, DisplayName = "Mesh Row")
class GAMECORE_API AGCMeshRow final : public AGCProceduralBase
{
	GENERATED_BODY()

public:

	AGCMeshRow();

	UPROPERTY(EditAnywhere, Category = "Settings")
		FRCStaticMeshProperties MeshData;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FRCPrimitiveCollision Collision;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 1, UIMin = 1))
		uint8 MeshCount;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		FVector MeshOffset;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		FTransform InstanceTransform;

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
