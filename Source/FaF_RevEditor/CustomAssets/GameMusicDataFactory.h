// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameMusic/GameMusicData.h"
#include "CustomAssets/GTAssetFactory.h"
#include "GameMusicDataFactory.generated.h"

UCLASS()
class UGameMusicDataFactory final : public UGTAssetFactory
{
	GENERATED_BODY()

public:

	UGameMusicDataFactory()
	{
		SupportedClass = UGameMusicData::StaticClass();
	}

	bool ConfigureProperties() override
	{
		DataClass = UGameMusicData::StaticClass();
		return DataClass ? true : false;
	}
};