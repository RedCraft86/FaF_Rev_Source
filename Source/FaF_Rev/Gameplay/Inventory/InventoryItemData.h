// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Data/PrimitiveData.h"
#include "Inventory/InventoryItemDataBase.h"
#include "InventoryItemData.generated.h"

UCLASS()
class FAF_REV_API UInventoryItemData : public UInventoryItemDataBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Inspection")
		FStaticMeshProperties InspectionData;

	UPROPERTY(EditAnywhere, Category = "Inspection")
		FTransform InspectionTransform;

	UPROPERTY(EditAnywhere, Category = "Inspection")
		FVector2D InspectionZoomRange;

private:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
