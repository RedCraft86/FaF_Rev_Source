// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "CustomAssets/GTAssetTypeActions.h"
#include "FaF_Rev/WorldMusic/WorldMusicData.h"

class FWorldMusicDataActions final : public FGTAssetTypeActions
{
public:
	
	explicit FWorldMusicDataActions() {}
	virtual FColor GetTypeColor() const override { return FColor(160, 25, 140); }
	virtual UClass* GetSupportedClass() const override { return UWorldMusicData::StaticClass(); }
	virtual FText GetName() const override { return NSLOCTEXT("FaF_Rev_Editor", "WorldMusicDataName", "World Music Data"); }
};