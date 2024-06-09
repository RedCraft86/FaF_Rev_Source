// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#include "GenericDataNode.h"
#include "GenericDataGraph.h"

UGenericDataNode::UGenericDataNode()
{
	UniqueID = FGuid::NewGuid();
	Graph = nullptr;
	ParentNodes = {};
	ChildNodes = {};
	Edges = {};
#if WITH_EDITORONLY_DATA
	bCanEditName = true;
	NodeTitle = NSLOCTEXT("DataGraphRuntime", "GenericDataNodeName", "Generic Data Node");
	NodeColor = FLinearColor::Black;
	ChildLimit = -1;
	ParentLimit = -1;
	CompatibleGraph = UGenericDataGraph::StaticClass();
	ContextMenuName = NodeTitle;
#endif
}

UGenericDataEdge* UGenericDataNode::GetEdge(UGenericDataNode* ChildNode) const
{
	TObjectPtr<UGenericDataEdge> const* FoundEdge = Edges.Find(ChildNode);
	return FoundEdge ? *FoundEdge : nullptr;
}

#if WITH_EDITOR
bool UGenericDataNode::CanCreateConnection(UGenericDataNode* Other, FString& ErrorMessage)
{
	ErrorMessage = TEXT("");
	return true;
}

bool UGenericDataNode::CanCreateConnectionTo(UGenericDataNode* Other, const int32 NumOfChildren, FString& ErrorMessage)
{
	if (ChildLimit >= 0 && NumOfChildren >= ChildLimit)
	{
		ErrorMessage = TEXT("Child Limit Exceeded");
		return false;
	}

	return CanCreateConnection(Other, ErrorMessage);
}

bool UGenericDataNode::CanCreateConnectionFrom(UGenericDataNode* Other, const int32 NumOfParents, FString& ErrorMessage)
{
	if (ParentLimit >= 0 && NumOfParents >= ParentLimit)
	{
		ErrorMessage = TEXT("Parent Limit Exceeded");
		return false;
	}

	return CanCreateConnection(Other, ErrorMessage);
}
#endif
