// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "EquipmentActor.h"
#include "ConsumableActor.h"
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
		, EquipmentClass(nullptr), bExpectSaveData(true), ConsumableClass(nullptr)
		, ConsumeDisplayText(INVTEXT("Use")), PreviewZoomRange(0.1f, 1.5f), MeshData({})
	{}

	UPROPERTY(EditAnywhere, Category = "ItemData", meta = (DisplayPriority = -9))
		uint8 Priority;

	UPROPERTY(EditAnywhere, Category = "ItemData", meta = (DisplayPriority = -9))
		FText DisplayName;

	UPROPERTY(EditAnywhere, Category = "ItemData", meta = (DisplayPriority = -9, MultiLine = true))
		FText Description;

	UPROPERTY(EditAnywhere, Category = "TypeData", meta = (InlineCategoryProperty))
		EInventoryItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "TypeData", meta = (EditCondition = "ItemType == EInventoryItemType::Viewable", EditConditionHides))
		TObjectPtr<UTexture2D> ViewImage;

	UPROPERTY(EditAnywhere, Category = "TypeData", meta = (EditCondition = "ItemType == EInventoryItemType::Viewable", EditConditionHides, MultiLine = true))
		FText ViewText;
	
	UPROPERTY(EditAnywhere, Category = "TypeData", meta = (EditCondition = "ItemType == EInventoryItemType::Equipment", EditConditionHides))
		TSubclassOf<AEquipmentActor> EquipmentClass;

	UPROPERTY(EditAnywhere, Category = "TypeData", meta = (EditCondition = "ItemType == EInventoryItemType::Equipment", EditConditionHides))
		bool bExpectSaveData;
	
	UPROPERTY(EditAnywhere, Category = "TypeData", meta = (EditCondition = "ItemType == EInventoryItemType::Consumable", EditConditionHides))
		TSubclassOf<AConsumableActor> ConsumableClass;

	UPROPERTY(EditAnywhere, Category = "TypeData", meta = (EditCondition = "ItemType == EInventoryItemType::Consumable", EditConditionHides))
		FText ConsumeDisplayText;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "DisplayMesh")
		bool bUpdate = false;
#endif
	
	UPROPERTY(EditAnywhere, Category = "DisplayMesh")
		FVector2D PreviewZoomRange;
	
	UPROPERTY(EditAnywhere, Category = "DisplayMesh")
		TArray<FTransformMeshData> MeshData;
	
	FString GetTypeString() const;
	FTransformMeshData GetMeshData(const TMap<FName, FString>& InMetadata) const;

#if WITH_EDITOR
private:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
