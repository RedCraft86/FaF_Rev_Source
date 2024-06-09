// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "CustomAssets/GTAssetFactory.h"
#include "Inventory/InventoryItemDataBase.h"
#include "ItemDataFactory.generated.h"

UCLASS()
class UItemDataFactory final : public UGTAssetFactory
{
	GENERATED_BODY()

public:

	UItemDataFactory()
	{
		SupportedClass = UInventoryItemDataBase::StaticClass();
	}

	bool ConfigureProperties() override
	{
		DataClass = UInventoryItemDataBase::DefaultSpawnClass;
		return DataClass ? true : false;
	}
};
