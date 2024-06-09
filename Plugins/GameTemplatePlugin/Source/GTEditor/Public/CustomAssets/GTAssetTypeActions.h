// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "AssetTypeActions_Base.h"

class FGTAssetTypeActions : public FAssetTypeActions_Base
{
public:

	explicit FGTAssetTypeActions() {}
	virtual uint32 GetCategories() override
	{
		if (const FAssetToolsModule* AssetToolsModule = FModuleManager::LoadModulePtr<FAssetToolsModule>("AssetTools"))
		{
			return AssetToolsModule->Get().RegisterAdvancedAssetCategory(TEXT("CustomTypes"),
				NSLOCTEXT("AssetCategory", "CustomAssetCategory", "Custom Assets"));
		}

		return EAssetTypeCategories::Misc;
	}
};
