// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Data/PrimitiveData.h"
#include "Inventory/InventoryItemDataBase.h"
#include "InventoryItemData.generated.h"

UENUM(BlueprintType)
enum class EInventoryItemType : uint8
{
	Objective,
	Viewable,
	Consumable,
	Equipment
};

UCLASS()
class FAF_REV_API UInventoryItemData final : public UInventoryItemDataBase
{
	GENERATED_BODY()

public:

	UInventoryItemData()
		: Priority(1), DisplayName(INVTEXT("Unknown Item")), Description(INVTEXT("Unknown Item"))
		, MeshZoomRange(0.1f, 1.5f), MeshData({})
	{}

	UPROPERTY(EditAnywhere, Category = "ItemData", meta = (DisplayPriority = -9))
		uint8 Priority;

	UPROPERTY(EditAnywhere, Category = "ItemData", meta = (DisplayPriority = -9))
		FText DisplayName;

	UPROPERTY(EditAnywhere, Category = "ItemData", meta = (MultiLine = true, DisplayPriority = -9))
		FText Description;
	
	UPROPERTY(EditAnywhere, Category = "ItemData|Inspection")
		FVector2D MeshZoomRange;
	
	UPROPERTY(EditAnywhere, Category = "ItemData|Inspection")
		TArray<FTransformMeshData> MeshData;

	FTransformMeshData GetMeshData(const TMap<FName, FString>& InMetadata) const;

private:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
