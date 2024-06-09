// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#include "EdGraph_DataGraph.h"
#include "EdNode_DataGraphEdge.h"
#include "EdNode_DataGraphNode.h"
#include "DataGraphEditor.h"

#include "GenericDataGraph.h"
#include "GenericDataNode.h"
#include "GenericDataEdge.h"

void UEdGraph_DataGraph::RebuildDataGraph()
{
	UGenericDataGraph* Graph = GetGraph();

	Clear();
	for (int i = 0; i < Nodes.Num(); ++i)
	{
		if (UEdNode_DataGraphNode* EdNode = Cast<UEdNode_DataGraphNode>(Nodes[i]))
		{
			if (EdNode->GraphNode == nullptr) continue;
			UGenericDataNode* DataGraphNode = EdNode->GraphNode;
			NodeMap.Add(DataGraphNode, EdNode);
			Graph->AllNodes.Add(DataGraphNode);
			
			for (int PinIdx = 0; PinIdx < EdNode->Pins.Num(); ++PinIdx)
			{
				UEdGraphPin* Pin = EdNode->Pins[PinIdx];
				if (Pin->Direction != EGPD_Output) continue;
				for (int LinkToIdx = 0; LinkToIdx < Pin->LinkedTo.Num(); ++LinkToIdx)
				{
					UGenericDataNode* ChildNode = nullptr;
					if (const UEdNode_DataGraphNode* EdNode_Child = Cast<UEdNode_DataGraphNode>(Pin->LinkedTo[LinkToIdx]->GetOwningNode()))
					{
						ChildNode = EdNode_Child->GraphNode;
					}
					else if (UEdNode_DataGraphEdge* EdNode_Edge = Cast<UEdNode_DataGraphEdge>(Pin->LinkedTo[LinkToIdx]->GetOwningNode()))
					{
						if (const UEdNode_DataGraphNode* Child = EdNode_Edge->GetEndNode())
						{
							ChildNode = Child->GraphNode;
						}
					}

					if (ChildNode)
					{
						DataGraphNode->ChildNodes.Add(ChildNode);
						ChildNode->ParentNodes.Add(DataGraphNode);
					}
					else
					{
						UE_LOG(DataGraphEditor, Error, TEXT("UEdGraph_DataGraph::RebuildDataGraph can't find child node"));
					}
				}
			}
		}
		else if (UEdNode_DataGraphEdge* EdgeNode = Cast<UEdNode_DataGraphEdge>(Nodes[i]))
		{
			const UEdNode_DataGraphNode* StartNode = EdgeNode->GetStartNode();
			const UEdNode_DataGraphNode* EndNode = EdgeNode->GetEndNode();
			UGenericDataEdge* Edge = EdgeNode->GraphEdge;

			if (StartNode == nullptr || EndNode == nullptr || Edge == nullptr)
			{
				UE_LOG(DataGraphEditor, Error, TEXT("UEdGraph_DataGraph::RebuildDataGraph add edge failed."));
				continue;
			}

			EdgeMap.Add(Edge, EdgeNode);

			Edge->Graph = Graph;
			Edge->Rename(nullptr, Graph, REN_DontCreateRedirectors | REN_DoNotDirty);
			Edge->StartNode = StartNode->GraphNode;
			Edge->EndNode = EndNode->GraphNode;
			Edge->StartNode->Edges.Add(Edge->EndNode, Edge);
		}
	}

	for (int i = 0; i < Graph->AllNodes.Num(); ++i)
	{
		UGenericDataNode* Node = Graph->AllNodes[i];
		if (Node->ParentNodes.IsEmpty())
		{
			Graph->RootNodes.Add(Node);
			SortNodes(Node);
		}

		Node->Graph = Graph;
		Node->Rename(nullptr, Graph, REN_DontCreateRedirectors | REN_DoNotDirty);
	}

	Graph->RootNodes.Sort([&](const UGenericDataNode& L, const UGenericDataNode& R)
	{
		const UEdNode_DataGraphNode* EdNode_LNode = NodeMap[&L];
		const UEdNode_DataGraphNode* EdNode_RNode = NodeMap[&R];
		return EdNode_LNode->NodePosX < EdNode_RNode->NodePosX;
	});
}

UGenericDataGraph* UEdGraph_DataGraph::GetGraph() const
{
	return CastChecked<UGenericDataGraph>(GetOuter());
}

bool UEdGraph_DataGraph::Modify(bool bAlwaysMarkDirty)
{
	const bool bModified = Super::Modify(bAlwaysMarkDirty);
	
	GetGraph()->Modify();
	for (int32 i = 0; i < Nodes.Num(); ++i)
	{
		Nodes[i]->Modify();
	}

	return bModified;
}

void UEdGraph_DataGraph::PostEditUndo()
{
	Super::PostEditUndo();
	NotifyGraphChanged();
}

void UEdGraph_DataGraph::Clear()
{
	UGenericDataGraph* Graph = GetGraph();

	Graph->ClearGraph();
	NodeMap.Reset();
	EdgeMap.Reset();

	for (int i = 0; i < Nodes.Num(); ++i)
	{
		if (const UEdNode_DataGraphNode* EdNode = Cast<UEdNode_DataGraphNode>(Nodes[i]))
		{
			if (UGenericDataNode* DataGraphNode = EdNode->GraphNode)
			{
				DataGraphNode->ParentNodes.Reset();
				DataGraphNode->ChildNodes.Reset();
				DataGraphNode->Edges.Reset();
			}
		}
	}
}

void UEdGraph_DataGraph::SortNodes(UGenericDataNode* RootNode)
{
	int Level = 0;
	TArray<UGenericDataNode*> CurrLevelNodes = { RootNode };
	TArray<UGenericDataNode*> NextLevelNodes;
	TSet<UGenericDataNode*> Visited;

	while (!CurrLevelNodes.IsEmpty())
	{
		for (int i = 0; i < CurrLevelNodes.Num(); ++i)
		{
			UGenericDataNode* Node = CurrLevelNodes[i];
			Visited.Add(Node);

			auto Comp = [&](const UGenericDataNode& L, const UGenericDataNode& R)
			{
				const UEdNode_DataGraphNode* EdNode_LNode = NodeMap[&L];
				const UEdNode_DataGraphNode* EdNode_RNode = NodeMap[&R];
				return EdNode_LNode->NodePosX < EdNode_RNode->NodePosX;
			};

			Node->ChildNodes.Sort(Comp);
			Node->ParentNodes.Sort(Comp);

			for (int j = 0; j < Node->ChildNodes.Num(); ++j)
			{
				if(!Visited.Contains(Node->ChildNodes[j]))
				{
					NextLevelNodes.Add(Node->ChildNodes[j]);
				}
			}
		}

		CurrLevelNodes = NextLevelNodes;
		NextLevelNodes.Reset();
		Level++;
	}
}
