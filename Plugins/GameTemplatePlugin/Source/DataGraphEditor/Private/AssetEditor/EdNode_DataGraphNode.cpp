// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#include "EdNode_DataGraphNode.h"
#include "EdGraph_DataGraph.h"

#include "GenericDataNode.h"

UEdNode_DataGraphNode::UEdNode_DataGraphNode()
{
	SNode = nullptr;
	GraphNode = nullptr;
	bCanRenameNode = true;
}

void UEdNode_DataGraphNode::SetDataGraphNode(UGenericDataNode* InNode)
{
	GraphNode = InNode;
}

UEdGraph_DataGraph* UEdNode_DataGraphNode::GetDataGraphEdGraph() const
{
	return Cast<UEdGraph_DataGraph>(GetGraph());
}

void UEdNode_DataGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, "MultipleNodes", FName(), TEXT("In"));
	CreatePin(EGPD_Output, "MultipleNodes", FName(), TEXT("Out"));
}

FText UEdNode_DataGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return GraphNode ? GraphNode->GetNodeTitle() : Super::GetNodeTitle(TitleType);
}

void UEdNode_DataGraphNode::PrepareForCopying()
{
	GraphNode->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
}

void UEdNode_DataGraphNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);
	if (!FromPin) return;
	
	if (GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
	{
		FromPin->GetOwningNode()->NodeConnectionListChanged();
	}
}

FLinearColor UEdNode_DataGraphNode::GetBackgroundColor() const
{
	return GraphNode ? GraphNode->GetBackgroundColor() : FLinearColor::Black;
}
