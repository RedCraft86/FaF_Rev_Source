// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#include "EdNode_DataGraphEdge.h"
#include "EdNode_DataGraphNode.h"

#include "GenericDataEdge.h"

UEdNode_DataGraphEdge::UEdNode_DataGraphEdge()
{
	Graph = nullptr;
	GraphEdge = nullptr;
	bCanRenameNode = true;
}

void UEdNode_DataGraphEdge::SetEdge(UGenericDataEdge* Edge)
{
	GraphEdge = Edge;
}

void UEdNode_DataGraphEdge::CreateConnections(UEdNode_DataGraphNode* Start, UEdNode_DataGraphNode* End)
{
	Pins[0]->Modify();
	Pins[0]->LinkedTo.Empty();

	Start->GetOutputPin()->Modify();
	Pins[0]->MakeLinkTo(Start->GetOutputPin());

	Pins[1]->Modify();
	Pins[1]->LinkedTo.Empty();

	End->GetInputPin()->Modify();
	Pins[1]->MakeLinkTo(End->GetInputPin());
}

void UEdNode_DataGraphEdge::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, TEXT("Edge"), FName(), TEXT("In"));
	Inputs->bHidden = true;
	
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, TEXT("Edge"), FName(), TEXT("Out"));
	Outputs->bHidden = true;
}

FText UEdNode_DataGraphEdge::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return GraphEdge ? GraphEdge->GetNodeTitle() : Super::GetNodeTitle(TitleType);
}

void UEdNode_DataGraphEdge::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (Pin->LinkedTo.IsEmpty())
	{
		Modify();
		if (UEdGraph* ParentGraph = GetGraph()) ParentGraph->Modify();
		DestroyNode();
	}
}

void UEdNode_DataGraphEdge::PrepareForCopying()
{
	GraphEdge->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
}

UEdNode_DataGraphNode* UEdNode_DataGraphEdge::GetStartNode()
{
	return Pins[0]->LinkedTo.IsEmpty() ? nullptr : Cast<UEdNode_DataGraphNode>(Pins[0]->LinkedTo[0]->GetOwningNode());
}

UEdNode_DataGraphNode* UEdNode_DataGraphEdge::GetEndNode()
{
	return Pins[1]->LinkedTo.IsEmpty() ? nullptr : Cast<UEdNode_DataGraphNode>(Pins[1]->LinkedTo[0]->GetOwningNode());
}
