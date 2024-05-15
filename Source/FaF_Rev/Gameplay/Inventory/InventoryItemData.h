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
		, ItemType(EInventoryItemType::Objective) , ViewImage(nullptr), ViewText(FText::GetEmpty())
		, EquipmentClass(nullptr), ConsumableClass(nullptr), ConsumeDisplayText(INVTEXT("Use"))
		, InspectZoomRange(0.1f, 1.5f), MeshData({})
	{}

	UPROPERTY(EditAnywhere, Category = "ItemData", meta = (DisplayPriority = -9))
		uint8 Priority;

	UPROPERTY(EditAnywhere, Category = "ItemData", meta = (DisplayPriority = -9))
		FText DisplayName;

	UPROPERTY(EditAnywhere, Category = "ItemData", meta = (MultiLine = true, DisplayPriority = -9))
		FText Description;

	UPROPERTY(EditAnywhere, Category = "ItemData", meta = (DisplayPriority = -9))
		EInventoryItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "ItemData|Viewable", meta = (EditCondition = "ItemType == EInventoryItemType::Viewable", EditConditionHides))
		TObjectPtr<UTexture2D> ViewImage;

	UPROPERTY(EditAnywhere, Category = "ItemData|Viewable", meta = (EditCondition = "ItemType == EInventoryItemType::Viewable", EditConditionHides, MultiLine = true))
		FText ViewText;
	
	UPROPERTY(EditAnywhere, Category = "ItemData|Equipment", meta = (EditCondition = "ItemType == EInventoryItemType::Equipment", EditConditionHides))
		TSubclassOf<class AEquipmentActor> EquipmentClass;
	
	UPROPERTY(EditAnywhere, Category = "ItemData|Consumable", meta = (EditCondition = "ItemType == EInventoryItemType::Consumable", EditConditionHides))
		TSubclassOf<class AConsumableActor> ConsumableClass;

	UPROPERTY(EditAnywhere, Category = "ItemData|Consumable", meta = (EditCondition = "ItemType == EInventoryItemType::Consumable", EditConditionHides))
		FText ConsumeDisplayText;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "ItemData|DisplayMesh")
		bool bUpdate;
#endif
	
	UPROPERTY(EditAnywhere, Category = "ItemData|DisplayMesh")
		FVector2D InspectZoomRange;
	
	UPROPERTY(EditAnywhere, Category = "ItemData|DisplayMesh")
		TArray<FTransformMeshData> MeshData;
	
	FString GetTypeString() const;
	FTransformMeshData GetMeshData(const TMap<FName, FString>& InMetadata) const;

private:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
