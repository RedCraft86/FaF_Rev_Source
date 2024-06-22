// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameMusic/MusicDataBase.h"
#include "CustomAssets/GTAssetFactory.h"
#include "MusicDataFactory.generated.h"

UCLASS()
class UMusicDataFactory final : public UGTAssetFactory
{
	GENERATED_BODY()

public:

	UMusicDataFactory()
	{
		SupportedClass = UMusicDataBase::StaticClass();
	}

	bool ConfigureProperties() override
	{
		DataClass = UMusicDataBase::DefaultSpawnClass;
		return DataClass ? true : false;
	}
};
