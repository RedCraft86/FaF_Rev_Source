// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Inventory/InventoryComponentBase.h"
#include "InventoryComponent.generated.h"

UCLASS(ClassGroup = (Game))
class FAF_REV_API UInventoryComponent : public UInventoryComponentBase
{
	GENERATED_BODY()

	friend class AInspectionManager;

public:

	UInventoryComponent();

};
