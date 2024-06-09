// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

#include "AssetTypeActions_Base.h"
#include "GenericDataGraph.h"

class FAssetTypeActions_DataGraph final : public FAssetTypeActions_Base
{
public:
	
	explicit FAssetTypeActions_DataGraph() {}
	
	virtual uint32 GetCategories() override;
	virtual FColor GetTypeColor() const override { return FColor(100, 160, 80); }
	virtual UClass* GetSupportedClass() const override { return UGenericDataGraph::StaticClass(); }
	virtual FText GetName() const override { return NSLOCTEXT("DataGraphEditor", "GenericName", "Data Graph"); }
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
};
