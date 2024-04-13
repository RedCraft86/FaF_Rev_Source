// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "FaF_Rev/GameMusic/GameMusicData.h"
#include "CustomAssets/GTAssetTypeActions.h"

class FGameMusicDataActions final : public FGTAssetTypeActions
{
public:
	
	explicit FGameMusicDataActions() {}
	virtual FColor GetTypeColor() const override { return FColor(160, 25, 140); }
	virtual UClass* GetSupportedClass() const override { return UGameMusicData::StaticClass(); }
	virtual FText GetName() const override { return NSLOCTEXT("FaF_Rev_Editor", "GameMusicDataName", "Game Music Data"); }
};