// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#include "GenericDataGraph.h"
#include "GenericDataEdge.h"
#include "GenericDataNode.h"

UGenericDataGraph::UGenericDataGraph()
{
	EdgeType = UGenericDataEdge::StaticClass();
	NodeType = UGenericDataNode::StaticClass();
#if WITH_EDITORONLY_DATA
	EdGraph = nullptr;
	bCanBeCyclical = false;
#endif
}

int32 UGenericDataGraph::GetDepth() const
{
	int32 Depth = 0;
	TArray<UGenericDataNode*> NextLevel;
	TArray<UGenericDataNode*> CurrentLevel = RootNodes;
	while (!CurrentLevel.IsEmpty())
	{
		for (const UGenericDataNode* Node : CurrentLevel)
		{
			if (!Node) continue;
			NextLevel.Append(Node->ChildNodes);
		}
		
		CurrentLevel = NextLevel;
		NextLevel.Reset();
		Depth++;
	}

	NextLevel.Empty();
	CurrentLevel.Empty();
	return Depth;
}

int32 UGenericDataGraph::GetEndings() const
{
	int32 Endings = 0;
	for (const UGenericDataNode* Node : AllNodes)
	{
		if (Node && Node->IsLeafNode()) Endings++;
	}
	return Endings;
}

TArray<uint8> UGenericDataGraph::ValidateSequence(const TArray<uint8>& InSequence) const
{
	if (RootNodes.IsEmpty()) return {};
	if (InSequence.IsEmpty()) return {0};
	
	TArray<uint8> OutSequence = {};
	TArray<UGenericDataNode*> ChildNodes = RootNodes;
	for (const int32 NodeIdx : InSequence)
	{
		if (ChildNodes.IsEmpty() || !ChildNodes.IsValidIndex(NodeIdx)) break;
		const UGenericDataNode* Temp = ChildNodes[NodeIdx];
		if (!Temp) break;
		
		ChildNodes = Temp->ChildNodes;
		OutSequence.Add(NodeIdx);
	}

	ChildNodes.Empty();
	return OutSequence;
}

UGenericDataNode* UGenericDataGraph::GetNodeBySequence(const TArray<uint8>& InSequence) const
{
	if (RootNodes.IsEmpty()) return nullptr;
		
	UGenericDataNode* LastNode = nullptr;
	TArray<UGenericDataNode*> ChildNodes = RootNodes;
	for (const int32 NodeIdx : InSequence)
	{
		if (ChildNodes.IsEmpty() || !ChildNodes.IsValidIndex(NodeIdx)) break;
		UGenericDataNode* Temp = ChildNodes[NodeIdx];
		if (!Temp) break;
		
		ChildNodes = Temp->ChildNodes;
		LastNode = Temp;
	}

	ChildNodes.Empty();
	return LastNode;
}

TSet<UGenericDataNode*> UGenericDataGraph::GetNodesInSequence(const TArray<uint8>& InSequence) const
{
	if (RootNodes.IsEmpty()) return {};
	
	TSet<UGenericDataNode*> Nodes;
	TArray<UGenericDataNode*> ChildNodes = RootNodes;
	for (const int32 NodeIdx : InSequence)
	{
		if (ChildNodes.IsEmpty() || !ChildNodes.IsValidIndex(NodeIdx)) break;
		UGenericDataNode* Temp = ChildNodes[NodeIdx];
		if (!Temp) break;
		
		ChildNodes = Temp->ChildNodes;
		Nodes.Add(Temp);
	}

	ChildNodes.Empty();
	return Nodes;
}

TArray<uint8> UGenericDataGraph::GetSequenceFromNode(UGenericDataNode* InNode) const
{
	if (!InNode || InNode->Graph != this) return {};
	
	TArray<uint8> Sequence;
	UGenericDataNode* ChildNode = InNode;
	TArray<UGenericDataNode*> ParentNodes = InNode->ParentNodes;
	while(!ParentNodes.IsEmpty())
	{
		for (UGenericDataNode* ParentNode : ParentNodes)
		{
			const int32 Idx = ParentNode->ChildNodes.Find(ChildNode);
			if (Idx >= 0)
			{
				Sequence.Insert(Idx, 0);
				ParentNodes = ParentNode->ParentNodes;
				ChildNode = ParentNode;
				break;
			}
		}
	}

	Sequence.Insert(RootNodes.Find(ChildNode), 0);
	return Sequence;
}

TArray<uint8> UGenericDataGraph::GetSequenceFromNodeID(const FGuid& InNodeID) const
{
	if (!InNodeID.IsValid()) return {};
	for (UGenericDataNode* Node : AllNodes)
	{
		if (Node && Node->UniqueID == InNodeID)
		{
			return GetSequenceFromNode(Node);
		}
	}

	return {};
}

void UGenericDataGraph::ClearGraph()
{
	for (UGenericDataNode* Node : AllNodes)
	{
		if (!Node) continue;
		Node->ParentNodes.Empty();
		Node->ChildNodes.Empty();
		Node->Edges.Empty();
	}

	AllNodes.Empty();
	RootNodes.Empty();
}
