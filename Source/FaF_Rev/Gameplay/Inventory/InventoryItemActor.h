// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTActor.h"
#include "Data/PrimitiveData.h"
#include "Interaction/InteractionInterface.h"
#include "InventoryItemActor.generated.h"

UCLASS()
class FAF_REV_API AInventoryItemActor final : public AGTActor, public IInteractionInterface
{
	GENERATED_BODY()

public:

	AInventoryItemActor();

	UPROPERTY(VisibleDefaultsOnly, Category = "Subobject")
		TObjectPtr<class UBoxComponent> CollisionBox;

	UPROPERTY(VisibleDefaultsOnly, Category = "Subobject")
		TObjectPtr<UInstancedStaticMeshComponent> InstancedStaticMesh;

	UPROPERTY(EditAnywhere, Category = "Settings", DisplayName = "Item", meta = (DisplayThumbnail = false))
		TObjectPtr<class UInventoryItemData> ItemData;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 1, UIMin = 1))
		int32 Amount;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (MultiLine = true))
		TMap<FName, FString> Metadata;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (MakeEditWidget = true))
		TArray<FTransform> MeshInstances;

	// Inject Variables: {Item}, {Amount}, {m + Meta Key} 
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Settings")
		FInteractionInfo InteractionInfo;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Settings", meta = (MakeEditWidget = true))
		FVector BoxExtent;
	
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Settings")
		FPrimitiveCollision BoxCollision;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void FillAmount(const uint8 InAmount);

protected:

	FFormatNamedArguments MetaInteractArgs;

	virtual void SetEnabled(const bool bInEnabled) override;
	virtual bool GetInteractionInfo_Implementation(FInteractionInfo& Info) override;
	virtual void OnBeginInteract_Implementation(AFRPlayerBase* Player, const FHitResult& HitResult) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
};
