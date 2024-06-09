// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#include "DataGraphNodeFactory.h"
#include "EdNode_DataGraphEdge.h"
#include "EdNode_DataGraphNode.h"
#include "SNode_DataGraphEdge.h"
#include "SNode_DataGraphNode.h"

TSharedPtr<SGraphNode> FDataGraphNodeFactory::CreateNode(UEdGraphNode* Node) const
{
	if (UEdNode_DataGraphNode* EdNode_GraphNode = Cast<UEdNode_DataGraphNode>(Node))
	{
		return SNew(SNode_DataGraphNode, EdNode_GraphNode);
	}
	
	if (UEdNode_DataGraphEdge* EdNode_Edge = Cast<UEdNode_DataGraphEdge>(Node))
	{
		return SNew(SNode_DataGraphEdge, EdNode_Edge);
	}
	
	return FGraphPanelNodeFactory::CreateNode(Node);
}
