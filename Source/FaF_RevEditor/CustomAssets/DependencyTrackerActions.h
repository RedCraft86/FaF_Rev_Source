// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "CustomAssets/GTAssetTypeActions.h"
#include "FaF_Rev/GameSection/DependencyTracker.h"

class FDependencyTrackerActions final : public FGTAssetTypeActions
{
public:
	
	explicit FDependencyTrackerActions() {}
	virtual FColor GetTypeColor() const override { return FColor(160, 25, 140); }
	virtual UClass* GetSupportedClass() const override { return UDependencyTracker::StaticClass(); }
	virtual FText GetName() const override { return NSLOCTEXT("FaF_Rev_Editor", "DependencyTrackerName", "Dependency Tracker"); }
};
