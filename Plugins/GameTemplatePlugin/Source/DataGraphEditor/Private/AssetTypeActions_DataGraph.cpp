// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#include "AssetTypeActions_DataGraph.h"
#include "DataGraphAssetEditor.h"

uint32 FAssetTypeActions_DataGraph::GetCategories()
{
	if (const FAssetToolsModule* AssetToolsModule = FModuleManager::LoadModulePtr<FAssetToolsModule>("AssetTools"))
	{
		return AssetToolsModule->Get().RegisterAdvancedAssetCategory(TEXT("CustomTypes"),
			NSLOCTEXT("AssetCategory", "CustomAssetCategory", "Custom Assets"));
	}

	return EAssetTypeCategories::Misc;
}

void FAssetTypeActions_DataGraph::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ?
		EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	
	for (UObject* Object : InObjects)
	{
		if (UGenericDataGraph* Graph = Cast<UGenericDataGraph>(Object))
		{
			const TSharedRef<FDataGraphAssetEditor> GraphEditor(new FDataGraphAssetEditor());
			GraphEditor->InitEditor(Mode, EditWithinLevelEditor, Graph);
		}
	}
}
