// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

#include "AutoLayoutStrategy.h"
#include "ForcedDirectLayoutStrategy.generated.h"

UCLASS()
class UForcedDirectLayoutStrategy final : public UAutoLayoutStrategy
{
	GENERATED_BODY()
	
public:
	
	UForcedDirectLayoutStrategy();

	virtual void Layout(UEdGraph* InGraph) override;

private:

	bool bRandomInit;
	float InitTemperature;
	float CoolDownRate;

	FBox2D LayoutOneTree(UGenericDataNode* RootNode, const FBox2D& PreTreeBound);
};