// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "EquipmentActor.h"
#include "ConsumableActor.h"
#include "Data/PrimitiveData.h"
#include "Inventory/InventoryItemDataBase.h"
#include "InventoryItemData.generated.h"

namespace NativeItemKeys
{
	inline static FName KeyID			= TEXT("KeyID"); // ID used for Keys
	inline static FName AltMeshID		= TEXT("AltMeshID"); // Alt index of the mesh to use in the preview
	inline static FName SingleKey		= TEXT("SingleKey"); // Implies that a Key is only for one lock and should be consumed after use
	inline static FName DisplayName		= TEXT("DisplayName");
	inline static FName Description		= TEXT("Description");
	
	inline static TSet All = {KeyID, AltMeshID, SingleKey, DisplayName, Description};
}

UENUM(BlueprintType)
enum class EInventoryItemType : uint8
{
	Any UMETA(Hidden),
	Basic,
	Objective,
	Viewable,
	Consumable,
	Equipment
};
ENUM_RANGE_BY_FIRST_AND_LAST(EInventoryItemType, EInventoryItemType::Any, EInventoryItemType::Equipment);
[[nodiscard]] FAF_REV_API inline FString LexToString(const EInventoryItemType Type)
{
	switch(Type)
	{
	case EInventoryItemType::Any: return TEXT("Any/All");
	case EInventoryItemType::Basic: return TEXT("Generic");
	case EInventoryItemType::Objective: return TEXT("Key Items");
	case EInventoryItemType::Consumable: return TEXT("Consumables");
	case EInventoryItemType::Equipment: return TEXT("Equipments");
	case EInventoryItemType::Viewable: return TEXT("Pictures/Notes");
	}
	
	return TEXT("Unknown");
}

UCLASS()
class FAF_REV_API UInventoryItemData final : public UInventoryItemDataBase
{
	GENERATED_BODY()

public:

	UInventoryItemData()
		: Priority(255), DisplayName(INVTEXT("Unknown Item")), Description(INVTEXT("Unknown Item"))
		, ItemType(EInventoryItemType::Basic), ViewImage(nullptr), ViewText(FText::GetEmpty())
		, EquipmentClass(nullptr), bExpectSaveData(true), ConsumableClass(nullptr)
		, ConsumeDisplayText(INVTEXT("Use")), PreviewZoomRange(0.1f, 1.5f)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData", meta = (DisplayPriority = -9))
		uint8 Priority;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData", meta = (DisplayPriority = -9))
		FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData", meta = (DisplayPriority = -9, MultiLine = true))
		FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, BlueprintReadOnly, Category = "TypeData", meta = (DisplayPriority = -1))
		EInventoryItemType ItemType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TypeData", meta = (EditCondition = "ItemType == EInventoryItemType::Viewable", EditConditionHides))
		TObjectPtr<UTexture2D> ViewImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TypeData", meta = (EditCondition = "ItemType == EInventoryItemType::Viewable", EditConditionHides, MultiLine = true))
		FText ViewText;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TypeData", meta = (EditCondition = "ItemType == EInventoryItemType::Equipment", EditConditionHides))
		TSubclassOf<AEquipmentActor> EquipmentClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TypeData", meta = (EditCondition = "ItemType == EInventoryItemType::Equipment", EditConditionHides))
		bool bExpectSaveData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TypeData", meta = (EditCondition = "ItemType == EInventoryItemType::Consumable", EditConditionHides))
		TSubclassOf<AConsumableActor> ConsumableClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TypeData", meta = (EditCondition = "ItemType == EInventoryItemType::Consumable", EditConditionHides))
		FText ConsumeDisplayText;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "DisplayMesh")
		bool bUpdate = false;
#endif
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DisplayMesh", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		FVector2D PreviewZoomRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DisplayMesh")
		FTransformMeshData BaseMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DisplayMesh")
		TMap<FString, FTransformMeshData> AltMeshes;

	UFUNCTION(BlueprintPure, Category = "InventoryItem")
		FText GetDisplayName(const TMap<FName, FString>& InMetadata) const;
	
	UFUNCTION(BlueprintPure, Category = "InventoryItem")
		FText GetDescription(const TMap<FName, FString>& InMetadata) const;
	
	UFUNCTION(BlueprintPure, Category = "InventoryItem")
		FTransformMeshData GetMeshData(const TMap<FName, FString>& InMetadata) const;

#if WITH_EDITOR
private:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
