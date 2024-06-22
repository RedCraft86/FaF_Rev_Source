// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameMusic/MusicDataBase.h"
#include "CustomAssets/GTAssetTypeActions.h"

class FMusicDataAssetTypeActions final : public FGTAssetTypeActions
{
public:
	
	explicit FMusicDataAssetTypeActions() {}
	virtual FColor GetTypeColor() const override { return FColor(25, 100, 160); }
	virtual UClass* GetSupportedClass() const override { return UMusicDataBase::StaticClass(); }
	virtual FText GetName() const override { return NSLOCTEXT("GTEditor", "MusicDataName", "World Music Data"); }
};
