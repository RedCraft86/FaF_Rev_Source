// Copyright (C) RedCraft86. All Rights Reserved.

#include "FaF_Rev.h"
#include "Inventory/InventoryItemData.h"
#include "GameSection/GameSectionData.h"
#include "GameSection/GameSectionGraph.h"

DEFINE_LOG_CATEGORY(FaFRev);

void FFRGameModule::StartupModule()
{
	UInventoryItemDataBase::DefaultSpawnClass = UInventoryItemData::StaticClass();
	UGameSectionGraph::GraphNodeType = UGameSectionDataNode::StaticClass();
}

void FFRGameModule::ShutdownModule()
{

}

IMPLEMENT_PRIMARY_GAME_MODULE(FFRGameModule, FaF_Rev, "FaF_Rev");