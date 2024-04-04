// Copyright (C) RedCraft86. All Rights Reserved.

#include "FaF_Rev.h"
#include "Inventory/InventoryItemData.h"
#include "GameSection/GameSectionData.h"

void FFRGameModule::StartupModule()
{
	UInventoryItemDataBase::DefaultSpawnClass = UInventoryItemData::StaticClass();
	UGameSectionDataBase::DefaultSpawnClass = UGameSectionData::StaticClass();
}

void FFRGameModule::ShutdownModule()
{

}

IMPLEMENT_PRIMARY_GAME_MODULE(FFRGameModule, FaF_Rev, "FaF_Rev");
