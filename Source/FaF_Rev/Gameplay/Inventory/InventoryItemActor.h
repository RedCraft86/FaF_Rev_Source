﻿// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Data/PrimitiveData.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractionInterface.h"
#include "InventoryItemActor.generated.h"

UCLASS()
class FAF_REV_API AInventoryItemActor final : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:

	AInventoryItemActor();

	UPROPERTY(VisibleDefaultsOnly, Category = "Subobject")
		TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleDefaultsOnly, Category = "Subobject")
		TObjectPtr<class UBoxComponent> BoxCollision;

	UPROPERTY(VisibleDefaultsOnly, Category = "Subobject")
		TObjectPtr<UInstancedStaticMeshComponent> InstancedStaticMesh;

	UPROPERTY(EditAnywhere, Category = "Settings")
		TObjectPtr<class UInventoryItemData> ItemData;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 1, UIMin = 1))
		uint8 Amount;

	UPROPERTY(EditAnywhere, Category = "Settings")
		TMap<FName, FString> Metadata;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Settings")
		FInteractionInfo InteractionInfo;
	
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Settings", meta = (MakeEditWidget = true))
		FVector BoxExtent;
	
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Settings")
		FPrimitiveCollision CollisionData;
	
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Settings", meta = (MakeEditWidget = true))
		TArray<FTransform> MeshInstances;

protected:
	
	virtual bool GetInteractionInfo_Implementation(FInteractionInfo& Info) override;
	virtual void OnBeginInteract_Implementation(AFRPlayerBase* Player, const FHitResult& HitResult) override;
	virtual void OnConstruction(const FTransform& Transform) override;
};
