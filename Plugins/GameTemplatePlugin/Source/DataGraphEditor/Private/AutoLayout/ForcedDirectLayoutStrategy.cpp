// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#include "ForcedDirectLayoutStrategy.h"
#include "DataGraphEditorSettings.h"
#include "EdGraph_DataGraph.h"
#include "EdNode_DataGraphNode.h"

#include "GenericDataGraph.h"
#include "GenericDataNode.h"

static float CoolDown(const float Temp, const float CoolDownRate)
{
	if (Temp < .01) return .01;
	return Temp - (Temp / CoolDownRate);
}

static float GetRepulseForce(const float X, const float k)
{
	return X != 0 ? k * k / X : TNumericLimits<float>::Max();
}

UForcedDirectLayoutStrategy::UForcedDirectLayoutStrategy()
{
	bRandomInit = false;
	CoolDownRate = 10;
	InitTemperature = 10.f;
}

void UForcedDirectLayoutStrategy::Layout(UEdGraph* InGraph)
{
	EdGraph = Cast<UEdGraph_DataGraph>(InGraph);
	check(EdGraph);

	EdGraph->RebuildDataGraph();
	Graph = EdGraph->GetGraph();
	check(Graph);

	if (const UDataGraphEditorSettings* Settings = UDataGraphEditorSettings::Get())
	{
		OptimalDistance = Settings->OptimalDistance;
		MaxIteration = Settings->MaxIteration;
		bRandomInit = Settings->bRandomInit;
	}

	FBox2D PreTreeBound(ForceInitToZero);
	for (int32 i = 0; i < Graph->RootNodes.Num(); ++i)
	{
		PreTreeBound = LayoutOneTree(Graph->RootNodes[i], PreTreeBound);
	}
}

FBox2D UForcedDirectLayoutStrategy::LayoutOneTree(UGenericDataNode* RootNode, const FBox2D& PreTreeBound)
{
	float Temp = InitTemperature;
	FBox2D TreeBound = GetActualBounds(RootNode);
	TreeBound.Min.X += PreTreeBound.Max.X + OptimalDistance;
	TreeBound.Max.X += PreTreeBound.Max.X + OptimalDistance;
	if (bRandomInit) RandomLayoutOneTree(RootNode, TreeBound);

	float RepulseForce, Distance;
	FVector2D Diff;

	TMap<UEdGraphNode*, FVector2D> NodeToDisplacement;

	for (int32 i = 0; i < EdGraph->Nodes.Num(); ++i)
	{
		NodeToDisplacement.Add(EdGraph->Nodes[i], FVector2D(0.f, 0.f));
	}

	for (int32 IterationNum = 0; IterationNum < MaxIteration; ++IterationNum)
	{
		for (int32 i = 0; i < EdGraph->Nodes.Num(); ++i)
		{
			for (int32 j = 0; j < EdGraph->Nodes.Num(); ++j)
			{
				if (i == j) continue;
				Diff.X = EdGraph->Nodes[i]->NodePosX - EdGraph->Nodes[j]->NodePosX;
				Diff.Y = EdGraph->Nodes[i]->NodePosY - EdGraph->Nodes[j]->NodePosY;
				Distance = Diff.Size();
				Diff.Normalize();

				RepulseForce = Distance > 2 * OptimalDistance ? 0 : GetRepulseForce(Distance, OptimalDistance);

				NodeToDisplacement[EdGraph->Nodes[i]] += Diff * RepulseForce;
			}
		}

		int Level = 0;
		TArray<UGenericDataNode*> CurrLevelNodes = { RootNode };
		TArray<UGenericDataNode*> NextLevelNodes;
		while (!CurrLevelNodes.IsEmpty())
		{
			for (int32 i = 0; i < CurrLevelNodes.Num(); ++i)
			{
				UGenericDataNode* Node = CurrLevelNodes[i];
				check(Node);

				UEdNode_DataGraphNode* EdNode_ParentNode = EdGraph->NodeMap[Node];

				for (int32 j = 0; j < Node->ChildNodes.Num(); ++j)
				{
					NextLevelNodes.Add(Node->ChildNodes[j]);

					UEdNode_DataGraphNode* EdNode_ChildNode = EdGraph->NodeMap[Node->ChildNodes[j]];

					Diff.X = EdNode_ChildNode->NodePosX - EdNode_ParentNode->NodePosY;
					Diff.Y = EdNode_ChildNode->NodePosY - EdNode_ParentNode->NodePosY;
					Distance = Diff.Size();
					Diff.Normalize();
					
					NodeToDisplacement[EdNode_ParentNode] += Distance * Diff;
					NodeToDisplacement[EdNode_ChildNode] -= Distance * Diff;
				}
			}

			CurrLevelNodes = NextLevelNodes;
			NextLevelNodes.Reset();
			Level++;
		}

		for (int32 i = 0; i < EdGraph->Nodes.Num(); ++i)
		{
			UEdGraphNode* EdNode = EdGraph->Nodes[i];
			Distance = NodeToDisplacement[EdNode].Size();
			NodeToDisplacement[EdNode].Normalize();

			float Minimum = Distance < Temp ? Distance : Temp;
			EdNode->NodePosX += NodeToDisplacement[EdNode].X * Minimum;
			EdNode->NodePosY += NodeToDisplacement[EdNode].Y * Minimum;
		}

		Temp = CoolDown(Temp, CoolDownRate);
	}

	FBox2D ActualBound = GetActualBounds(RootNode);
	FVector2D Center = ActualBound.GetCenter();
	FVector2D TreeCenter = TreeBound.GetCenter();
	FVector2D Scale = (TreeBound.Max - TreeBound.Min) / (ActualBound.Max - ActualBound.Min);
	for (int32 i = 0; i < EdGraph->Nodes.Num(); ++i)
	{
		UEdGraphNode* EdNode = EdGraph->Nodes[i];
		EdNode->NodePosX = TreeCenter.X + Scale.X * (EdNode->NodePosX - Center.X);
		EdNode->NodePosY = TreeCenter.Y + Scale.Y * (EdNode->NodePosY - Center.Y);
	}

	return TreeBound;
}
