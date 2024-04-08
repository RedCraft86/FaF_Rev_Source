// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "CustomAssets/GTAssetFactory.h"
#include "FaF_Rev/GameSection/DependencyTracker.h"
#include "DependencyTrackerFactory.generated.h"

UCLASS()
class UDependencyTrackerFactory final : public UGTAssetFactory
{
	GENERATED_BODY()

public:

	UDependencyTrackerFactory()
	{
		SupportedClass = UDependencyTracker::StaticClass();
	}

	bool ConfigureProperties() override
	{
		DataClass = UDependencyTracker::StaticClass();
		return DataClass ? true : false;
	}
};
