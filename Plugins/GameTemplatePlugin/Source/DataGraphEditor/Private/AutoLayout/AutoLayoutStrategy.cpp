// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#include "AutoLayoutStrategy.h"
#include "EdNode_DataGraphNode.h"
#include "EdGraph_DataGraph.h"

#include "GenericDataNode.h"
#include "SNode_DataGraphNode.h"
#include "Kismet/KismetMathLibrary.h"

UAutoLayoutStrategy::UAutoLayoutStrategy()
{
	Graph = nullptr;
	EdGraph = nullptr;
	MaxIteration = 50;
	OptimalDistance = 150;
}

FBox2D UAutoLayoutStrategy::GetNodeBound(UEdGraphNode* EdNode) const
{
	const int32 NodeWidth = GetNodeWidth(Cast<UEdNode_DataGraphNode>(EdNode));
	const int32 NodeHeight = GetNodeHeight(Cast<UEdNode_DataGraphNode>(EdNode));
	const FVector2D Max(EdNode->NodePosX + NodeWidth, EdNode->NodePosY + NodeHeight);
	const FVector2D Min(EdNode->NodePosX, EdNode->NodePosY);
	return FBox2D(Min, Max);
}

int32 UAutoLayoutStrategy::GetNodeWidth(const UEdNode_DataGraphNode* EdNode) const
{
	return EdNode->SNode->GetCachedGeometry().GetLocalSize().X;
}

int32 UAutoLayoutStrategy::GetNodeHeight(const UEdNode_DataGraphNode* EdNode) const
{
	return EdNode->SNode->GetCachedGeometry().GetLocalSize().Y;
}

FBox2D UAutoLayoutStrategy::GetActualBounds(UGenericDataNode* RootNode) const
{
	int Level = 0;
	TArray<UGenericDataNode*> CurrLevelNodes = { RootNode };
	TArray<UGenericDataNode*> NextLevelNodes;

	FBox2D Bounds = GetNodeBound(EdGraph->NodeMap[RootNode]);
	while (!CurrLevelNodes.IsEmpty())
	{
		for (int i = 0; i < CurrLevelNodes.Num(); ++i)
		{
			UGenericDataNode* Node = CurrLevelNodes[i];
			check(Node);

			Bounds += GetNodeBound(EdGraph->NodeMap[Node]);
			for (int j = 0; j < Node->ChildNodes.Num(); ++j)
			{
				NextLevelNodes.Add(Node->ChildNodes[j]);
			}
		}

		CurrLevelNodes = NextLevelNodes;
		NextLevelNodes.Reset();
		Level++;
	}
	
	return Bounds;
}

void UAutoLayoutStrategy::RandomLayoutOneTree(UGenericDataNode* RootNode, const FBox2D& Bound)
{
	int Level = 0;
	TArray<UGenericDataNode*> CurrLevelNodes = { RootNode };
	TArray<UGenericDataNode*> NextLevelNodes;

	while (!CurrLevelNodes.IsEmpty())
	{
		for (int i = 0; i < CurrLevelNodes.Num(); ++i)
		{
			UGenericDataNode* Node = CurrLevelNodes[i];
			check(Node);

			UEdNode_DataGraphNode* EdNode_Node = EdGraph->NodeMap[Node];

			EdNode_Node->NodePosX = UKismetMathLibrary::RandomFloatInRange(Bound.Min.X, Bound.Max.X);
			EdNode_Node->NodePosY = UKismetMathLibrary::RandomFloatInRange(Bound.Min.Y, Bound.Max.Y);

			for (int j = 0; j < Node->ChildNodes.Num(); ++j)
			{
				NextLevelNodes.Add(Node->ChildNodes[j]);
			}
		}

		CurrLevelNodes = NextLevelNodes;
		NextLevelNodes.Reset();
		Level++;
	}
}
