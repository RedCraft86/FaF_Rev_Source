// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "CustomAssets/GTAssetFactory.h"
#include "FaF_Rev/WorldMusic/WorldMusicData.h"
#include "WorldMusicDataFactory.generated.h"

UCLASS()
class UWorldMusicDataFactory final : public UGTAssetFactory
{
	GENERATED_BODY()

public:

	UWorldMusicDataFactory()
	{
		SupportedClass = UWorldMusicData::StaticClass();
	}

	bool ConfigureProperties() override
	{
		DataClass = UWorldMusicData::StaticClass();
		return DataClass ? true : false;
	}
};