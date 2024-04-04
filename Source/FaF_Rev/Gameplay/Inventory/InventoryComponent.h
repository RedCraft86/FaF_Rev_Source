// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Inventory/InventoryComponentBase.h"
#include "InventoryComponent.generated.h"

UCLASS(ClassGroup = (Game), meta = (BlueprintSpawnableComponent))
class FAF_REV_API UInventoryComponent final : public UInventoryComponentBase
{
	GENERATED_BODY()

public:

	UInventoryComponent();
};
