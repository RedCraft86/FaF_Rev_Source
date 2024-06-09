// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

#include "EdGraphUtilities.h"

class FDataGraphNodeFactory final : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<SGraphNode> CreateNode(UEdGraphNode* Node) const override;
};
