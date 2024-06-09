// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#include "TreeLayoutStrategy.h"
#include "DataGraphEditorSettings.h"
#include "EdNode_DataGraphNode.h"
#include "EdGraph_DataGraph.h"

#include "GenericDataGraph.h"
#include "GenericDataNode.h"

void UTreeLayoutStrategy::Layout(UEdGraph* InGraph)
{
	EdGraph = Cast<UEdGraph_DataGraph>(InGraph);
	check(EdGraph);

	EdGraph->RebuildDataGraph();
	Graph = EdGraph->GetGraph();
	check(Graph);

	bool bFirstPassOnly = false;
	if (const UDataGraphEditorSettings* Settings = UDataGraphEditorSettings::Get())
	{
		OptimalDistance = Settings->OptimalDistance;
		MaxIteration = Settings->MaxIteration;
		bFirstPassOnly = Settings->bFirstPassOnly;
	}

	const FVector2D Anchor(0.f, 0.f);
	for (int32 i = 0; i < Graph->RootNodes.Num(); ++i)
	{
		UGenericDataNode* RootNode = Graph->RootNodes[i];
		InitPass(RootNode, Anchor);

		if (!bFirstPassOnly)
		{
			for (int32 j = 0; j < MaxIteration; ++j)
			{
				if (!ResolveConflictPass(RootNode)) break;
			}
		}
	}

	for (int32 i = 0; i < Graph->RootNodes.Num(); ++i)
	{
		for (int32 j = 0; j < i; ++j)
		{
			ResolveConflict(Graph->RootNodes[j], Graph->RootNodes[i]);
		}
	}
}

void UTreeLayoutStrategy::InitPass(UGenericDataNode* RootNode, const FVector2D& Anchor)
{
	UEdNode_DataGraphNode* EdNode_RootNode = EdGraph->NodeMap[RootNode];

	FVector2D ChildAnchor(FVector2D(0.f, GetNodeHeight(EdNode_RootNode) + OptimalDistance + Anchor.Y));
	for (int32 i = 0; i < RootNode->ChildNodes.Num(); ++i)
	{
		UGenericDataNode* Child = RootNode->ChildNodes[i];
		const UEdNode_DataGraphNode* EdNode_ChildNode = EdGraph->NodeMap[Child];
		if (i > 0)
		{
			const UGenericDataNode* PreChild = RootNode->ChildNodes[i - 1];
			const UEdNode_DataGraphNode* EdNode_PreChildNode = EdGraph->NodeMap[PreChild];
			ChildAnchor.X += OptimalDistance + GetNodeWidth(EdNode_PreChildNode) / 2;
		}
		ChildAnchor.X += GetNodeWidth(EdNode_ChildNode) / 2;
		InitPass(Child, ChildAnchor);
	}

	const float NodeWidth = GetNodeWidth(EdNode_RootNode);

	EdNode_RootNode->NodePosY = Anchor.Y;
	if (RootNode->ChildNodes.IsEmpty())
	{
		EdNode_RootNode->NodePosX = Anchor.X - NodeWidth / 2;
	}
	else
	{
		UpdateParentNodePosition(RootNode);
	}
}

bool UTreeLayoutStrategy::ResolveConflictPass(UGenericDataNode* Node)
{
	bool HasConflict = false;
	for (int32 i = 0; i < Node->ChildNodes.Num(); ++i)
	{
		UGenericDataNode* Child = Node->ChildNodes[i];
		if (ResolveConflictPass(Child))
		{
			HasConflict = true;
		}
	}

	for (int32 i = 0; i < Node->ParentNodes.Num(); ++i)
	{
		UGenericDataNode* ParentNode = Node->ParentNodes[i];
		for (int32 j = 0; j < ParentNode->ChildNodes.Num(); ++j)
		{
			UGenericDataNode* LeftSibling = ParentNode->ChildNodes[j];
			if (LeftSibling == Node)
				break;
			if (ResolveConflict(LeftSibling, Node))
			{
				HasConflict = true;
			}
		}
	}

	return HasConflict;
}

bool UTreeLayoutStrategy::ResolveConflict(UGenericDataNode* LRoot, UGenericDataNode* RRoot)
{
	TArray<UEdNode_DataGraphNode*> RightContour, LeftContour;

	GetRightContour(LRoot, 0, RightContour);
	GetLeftContour(RRoot, 0, LeftContour);

	int32 MaxOverlapDistance = 0;
	const int32 Num = FMath::Min(LeftContour.Num(), RightContour.Num());
	for (int32 i = 0; i < Num; ++i)
	{
		if (RightContour.Contains(LeftContour[i]) || LeftContour.Contains(RightContour[i]))
			break;

		const int32 RightBound = RightContour[i]->NodePosX + GetNodeWidth(RightContour[i]);
		const int32 LeftBound = LeftContour[i]->NodePosX;
		const int32 Distance = RightBound + OptimalDistance - LeftBound;
		if (Distance > MaxOverlapDistance)
		{
			MaxOverlapDistance = Distance;
		}
	}

	if (MaxOverlapDistance > 0)
	{
		ShiftSubTree(RRoot, FVector2D(MaxOverlapDistance, 0.f));

		TArray<UGenericDataNode*> ParentNodes = RRoot->ParentNodes;
		TArray<UGenericDataNode*> NextParentNodes;
		while (!ParentNodes.IsEmpty())
		{
			for (int32 i = 0; i < ParentNodes.Num(); ++i)
			{
				UpdateParentNodePosition(ParentNodes[i]);

				NextParentNodes.Append(ParentNodes[i]->ParentNodes);
			}

			ParentNodes = NextParentNodes;
			NextParentNodes.Reset();
		}

		return true;
	}

	return false;
}

void UTreeLayoutStrategy::GetLeftContour(UGenericDataNode* RootNode, int32 Level, TArray<UEdNode_DataGraphNode*>& Contour)
{
	UEdNode_DataGraphNode* EdNode_Node = EdGraph->NodeMap[RootNode];
	if (Level >= Contour.Num())
	{
		Contour.Add(EdNode_Node);
	}
	else if (EdNode_Node->NodePosX < Contour[Level]->NodePosX)
	{
		Contour[Level] = EdNode_Node;
	}

	for (int32 i = 0; i < RootNode->ChildNodes.Num(); ++i)
	{
		GetLeftContour(RootNode->ChildNodes[i], Level + 1, Contour);
	}
}

void UTreeLayoutStrategy::GetRightContour(UGenericDataNode* RootNode, int32 Level, TArray<UEdNode_DataGraphNode*>& Contour)
{
	UEdNode_DataGraphNode* EdNode_Node = EdGraph->NodeMap[RootNode];
	if (Level >= Contour.Num())
	{
		Contour.Add(EdNode_Node);
	}
	else if (EdNode_Node->NodePosX + GetNodeWidth(EdNode_Node) > Contour[Level]->NodePosX + GetNodeWidth(Contour[Level]))
	{
		Contour[Level] = EdNode_Node;
	}

	for (int32 i = 0; i < RootNode->ChildNodes.Num(); ++i)
	{
		GetRightContour(RootNode->ChildNodes[i], Level + 1, Contour);
	}
}

void UTreeLayoutStrategy::ShiftSubTree(UGenericDataNode* RootNode, const FVector2D& Offset)
{
	UEdNode_DataGraphNode* EdNode_Node = EdGraph->NodeMap[RootNode];
	EdNode_Node->NodePosX += Offset.X;
	EdNode_Node->NodePosY += Offset.Y;

	for (int32 i = 0; i < RootNode->ChildNodes.Num(); ++i)
	{
		UGenericDataNode* Child = RootNode->ChildNodes[i];

		if (Child->ParentNodes[0] == RootNode)
		{
			ShiftSubTree(RootNode->ChildNodes[i], Offset);
		}
	}
}

void UTreeLayoutStrategy::UpdateParentNodePosition(UGenericDataNode* RootNode)
{
	UEdNode_DataGraphNode* EdNode_ParentNode = EdGraph->NodeMap[RootNode];
	if (RootNode->ChildNodes.Num() % 2 == 0)
	{
		const UEdNode_DataGraphNode* FirstChild = EdGraph->NodeMap[RootNode->ChildNodes[0]];
		UEdNode_DataGraphNode* LastChild = EdGraph->NodeMap[RootNode->ChildNodes.Last()];
		const float LeftBound = FirstChild->NodePosX;
		const float RightBound = LastChild->NodePosX + GetNodeWidth(LastChild);
		EdNode_ParentNode->NodePosX = (LeftBound + RightBound) / 2 - GetNodeWidth(EdNode_ParentNode) / 2;
	}
	else
	{
		UEdNode_DataGraphNode* MidChild = EdGraph->NodeMap[RootNode->ChildNodes[RootNode->ChildNodes.Num() / 2]];
		EdNode_ParentNode->NodePosX = MidChild->NodePosX + GetNodeWidth(MidChild) / 2 - GetNodeWidth(EdNode_ParentNode) / 2;
	}
}
