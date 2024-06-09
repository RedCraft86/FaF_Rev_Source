// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

#include "UObject/Object.h"
#include "AutoLayoutStrategy.generated.h"

class UEdNode_DataGraphNode;

UCLASS(Abstract)
class UAutoLayoutStrategy : public UObject
{
	GENERATED_BODY()
	
public:
	
	UAutoLayoutStrategy();

	virtual void Layout(UEdGraph* InGraph) {}

protected:
	
	int32 MaxIteration;
	int32 OptimalDistance;
	TObjectPtr<class UEdGraph_DataGraph> EdGraph;
	TObjectPtr<class UGenericDataGraph> Graph;
	
	FBox2D GetNodeBound(UEdGraphNode* EdNode) const;
	int32 GetNodeWidth(const UEdNode_DataGraphNode* EdNode) const;
	int32 GetNodeHeight(const UEdNode_DataGraphNode* EdNode) const;
	FBox2D GetActualBounds(class UGenericDataNode* RootNode) const;
	virtual void RandomLayoutOneTree(class UGenericDataNode* RootNode, const FBox2D& Bound);
};
