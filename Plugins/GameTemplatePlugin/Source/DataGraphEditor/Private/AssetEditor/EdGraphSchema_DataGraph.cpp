// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#include "EdGraphSchema_DataGraph.h"
#include "DataGraphConnectionPolicy.h"
#include "EdNode_DataGraphEdge.h"
#include "EdNode_DataGraphNode.h"
#include "EdGraph_DataGraph.h"

#include "GenericDataEdge.h"
#include "GenericDataNode.h"
#include "GenericDataGraph.h"
#include "GraphEditorActions.h"
#include "Framework/Commands/GenericCommands.h"

#define LOCTEXT_NAMESPACE "DataGraphEditor"

int32 UEdGraphSchema_DataGraph::CurrentCacheRefreshID = 0;

class FNodeVisitorCycleChecker
{
public:

	bool CheckForLoop(UEdGraphNode* StartNode, UEdGraphNode* EndNode)
	{
		VisitedNodes.Add(StartNode);
		return TraverseNodes(EndNode);
	}

private:
	
	bool TraverseNodes(UEdGraphNode* Node)
	{
		VisitedNodes.Add(Node);
		for (const UEdGraphPin* MyPin : Node->Pins)
		{
			if (MyPin->Direction == EGPD_Output)
			{
				for (const UEdGraphPin* OtherPin : MyPin->LinkedTo)
				{
					UEdGraphNode* OtherNode = OtherPin->GetOwningNode();
					if (VisitedNodes.Contains(OtherNode)) return false;
					if (!FinishedNodes.Contains(OtherNode))
					{
						if (!TraverseNodes(OtherNode)) return false;
					}
				}
			}
		}

		VisitedNodes.Remove(Node);
		FinishedNodes.Add(Node);
		return true;
	}
	
	TSet<UEdGraphNode*> VisitedNodes;
	TSet<UEdGraphNode*> FinishedNodes;
};

UEdGraphNode* FEdSchemaAction_DataGraph_NewNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = nullptr;
	if (NodeTemplate)
	{
		const FScopedTransaction Transaction(LOCTEXT("NewNode", "Data Graph Editor: New Node"));
		if (FromPin) FromPin->Modify();
		ParentGraph->Modify();

		NodeTemplate->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		NodeTemplate->GraphNode->SetFlags(RF_Transactional);
		NodeTemplate->SetFlags(RF_Transactional);

		ResultNode = NodeTemplate;
	}

	return ResultNode;
}

void FEdSchemaAction_DataGraph_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

UEdGraphNode* FEdSchemaAction_DataGraph_NewEdge::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = nullptr;
	if (NodeTemplate)
	{
		const FScopedTransaction Transaction(LOCTEXT("NewEdge", "Data Graph Editor: New Edge"));
		if (FromPin) FromPin->Modify();
		ParentGraph->Modify();

		NodeTemplate->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		NodeTemplate->GraphEdge->SetFlags(RF_Transactional);
		NodeTemplate->SetFlags(RF_Transactional);

		ResultNode = NodeTemplate;
	}
	
	return ResultNode;
}

void FEdSchemaAction_DataGraph_NewEdge::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

void UEdGraphSchema_DataGraph::GetBreakLinkToSubMenuActions(UToolMenu* Menu, UEdGraphPin* InGraphPin)
{
	TMap<FString, uint32> LinkTitleCount;
	FToolMenuSection& Section = Menu->FindOrAddSection("DataGraphAssetGraphSchemaPinActions");
	for (TArray<UEdGraphPin*>::TConstIterator Links(InGraphPin->LinkedTo); Links; ++Links)
	{
		const UEdGraphPin* Pin = *Links;
		FString TitleString = Pin->GetOwningNode()->GetNodeTitle(ENodeTitleType::ListView).ToString();
		FText Title = FText::FromString(TitleString);
		if (Pin->PinName != TEXT(""))
		{
			TitleString = FString::Printf(TEXT("%s (%s)"), *TitleString, *Pin->PinName.ToString());

			FFormatNamedArguments Args;
			Args.Add(TEXT("NodeTitle"), Title);
			Args.Add(TEXT("PinName"), Pin->GetDisplayName());
			Title = FText::Format(LOCTEXT("BreakDescPin", "{NodeTitle} ({PinName})"), Args);
		}

		uint32& Count = LinkTitleCount.FindOrAdd(TitleString);

		FText Description;
		FFormatNamedArguments Args;
		Args.Add(TEXT("NodeTitle"), Title);
		Args.Add(TEXT("NumberOfNodes"), Count);

		if (Count == 0)
		{
			Description = FText::Format(LOCTEXT("BreakDesc", "Break link to {NodeTitle}"), Args);
		}
		else
		{
			Description = FText::Format(LOCTEXT("BreakDescMulti", "Break link to {NodeTitle} ({NumberOfNodes})"), Args);
		}
		Count++;

		Section.AddMenuEntry(NAME_None, Description, Description, FSlateIcon(), FUIAction(
			FExecuteAction::CreateUObject(this, &UEdGraphSchema_DataGraph::BreakSinglePinLink, InGraphPin, *Links)));
	}
}

EGraphType UEdGraphSchema_DataGraph::GetGraphType(const UEdGraph* TestEdGraph) const
{
	return GT_StateMachine;
}

void UEdGraphSchema_DataGraph::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	UGenericDataGraph* Graph = CastChecked<UGenericDataGraph>(ContextMenuBuilder.CurrentGraph->GetOuter());
	if (!Graph->GetNodeType()) return;

	TSet<TSubclassOf<UGenericDataNode>> Visited;
	FText Desc = Graph->GetNodeType().GetDefaultObject()->ContextMenuName;
	const FText AddToolTip = LOCTEXT("NewNodeTooltip", "Add node here");
	
	if (Desc.IsEmpty())
	{
		FString Title = Graph->GetNodeType()->GetName();
		Title.RemoveFromEnd("_C");
		Desc = FText::FromString(Title);
	}

	if (!Graph->GetNodeType()->HasAnyClassFlags(CLASS_Abstract))
	{
		const TSharedPtr<FEdSchemaAction_DataGraph_NewNode> Action(new FEdSchemaAction_DataGraph_NewNode(LOCTEXT("NodeAction", "Data Graph Node"), Desc, AddToolTip, 0));
		Action->NodeTemplate = NewObject<UEdNode_DataGraphNode>(ContextMenuBuilder.OwnerOfTemporaries);
		Action->NodeTemplate->GraphNode = NewObject<UGenericDataNode>(Action->NodeTemplate, Graph->GetNodeType());
		Action->NodeTemplate->GraphNode->Graph = Graph;
		ContextMenuBuilder.AddAction(Action);

		Visited.Add(Graph->GetNodeType());
	}

	for (TObjectIterator<UClass> It; It; ++It)
	{
		if (It->IsChildOf(Graph->GetNodeType()) && !It->HasAnyClassFlags(CLASS_Abstract) && !Visited.Contains(*It))
		{
			TSubclassOf<UGenericDataNode> NodeType = *It;
			if (It->GetName().StartsWith("REINST") || It->GetName().StartsWith("SKEL")) continue;
			if (!Graph->GetClass()->IsChildOf(NodeType.GetDefaultObject()->CompatibleGraph)) continue;

			Desc = NodeType.GetDefaultObject()->ContextMenuName;

			if (Desc.IsEmpty())
			{
				FString Title = NodeType->GetName();
				Title.RemoveFromEnd("_C");
				Desc = FText::FromString(Title);
			}

			TSharedPtr<FEdSchemaAction_DataGraph_NewNode> Action(new FEdSchemaAction_DataGraph_NewNode(LOCTEXT("NodeAction", "Data Graph Node"), Desc, AddToolTip, 0));
			Action->NodeTemplate = NewObject<UEdNode_DataGraphNode>(ContextMenuBuilder.OwnerOfTemporaries);
			Action->NodeTemplate->GraphNode = NewObject<UGenericDataNode>(Action->NodeTemplate, NodeType);
			Action->NodeTemplate->GraphNode->Graph = Graph;
			ContextMenuBuilder.AddAction(Action);

			Visited.Add(NodeType);
		}
	}
}

void UEdGraphSchema_DataGraph::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	if (Context->Pin)
	{
		{
			FToolMenuSection& Section = Menu->AddSection("DataGraphSchemaActions", LOCTEXT("PinActionsMenuHeader", "Pin Actions"));
			if (!Context->Pin->LinkedTo.IsEmpty())
			{
				Section.AddMenuEntry(FGraphEditorCommands::Get().BreakPinLinks);
				if (Context->Pin->LinkedTo.Num() > 1)
				{
					Section.AddSubMenu("BreakLinkTo", LOCTEXT("BreakLinkTo", "Break Link To..."), LOCTEXT("BreakSpecificLinks", "Break a specific link..."),
						FNewToolMenuDelegate::CreateUObject((UEdGraphSchema_DataGraph* const)this, &UEdGraphSchema_DataGraph::GetBreakLinkToSubMenuActions, const_cast<UEdGraphPin*>(Context->Pin)));
				}
				else
				{
					((UEdGraphSchema_DataGraph* const)this)->GetBreakLinkToSubMenuActions(Menu, const_cast<UEdGraphPin*>(Context->Pin));
				}
			}
		}
	}
	else if (Context->Node)
	{
		{
			FToolMenuSection& Section = Menu->AddSection("DataGraphSchemaActions", LOCTEXT("ClassActionsMenuHeader", "Node Actions"));
			Section.AddMenuEntry(FGenericCommands::Get().Delete);
			Section.AddMenuEntry(FGenericCommands::Get().Cut);
			Section.AddMenuEntry(FGenericCommands::Get().Copy);
			Section.AddMenuEntry(FGenericCommands::Get().Duplicate);

			Section.AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
		}
	}

	Super::GetContextMenuActions(Menu, Context);
}

const FPinConnectionResponse UEdGraphSchema_DataGraph::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorSameNode", "Can't connect node to itself"));
	}

	const UEdGraphPin* Out = A;
	const UEdGraphPin* In = B;

	const UEdNode_DataGraphNode* EdNode_Out = Cast<UEdNode_DataGraphNode>(Out->GetOwningNode());
	const UEdNode_DataGraphNode* EdNode_In = Cast<UEdNode_DataGraphNode>(In->GetOwningNode());
	if (EdNode_Out == nullptr || EdNode_In == nullptr)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinError", "Not a valid UDataGraphEdNode"));
	}

	bool bAllowCycles = false;
	if (const UEdGraph_DataGraph* EdGraph = Cast<UEdGraph_DataGraph>(Out->GetOwningNode()->GetGraph()))
	{
		bAllowCycles = EdGraph->GetGraph()->bCanBeCyclical;
	}
	
	FNodeVisitorCycleChecker CycleChecker;
	if (!bAllowCycles && !CycleChecker.CheckForLoop(Out->GetOwningNode(), In->GetOwningNode()))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorCycle", "Can't create a graph cycle"));
	}

	FString ErrorMessage;
	if (!EdNode_Out->GraphNode->CanCreateConnectionTo(EdNode_In->GraphNode, EdNode_Out->GetOutputPin()->LinkedTo.Num(), ErrorMessage))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, ErrorMessage);
	}
	if (!EdNode_In->GraphNode->CanCreateConnectionFrom(EdNode_Out->GraphNode, EdNode_In->GetInputPin()->LinkedTo.Num(), ErrorMessage))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, ErrorMessage);
	}
	
	if (EdNode_Out->GraphNode->GetGraph()->GetEdgeType() != NULL)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE, LOCTEXT("PinConnect", "Connect nodes with edge"));
	}
	
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, LOCTEXT("PinConnect", "Connect nodes"));
}

bool UEdGraphSchema_DataGraph::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	const UEdNode_DataGraphNode* NodeA = Cast<UEdNode_DataGraphNode>(A->GetOwningNode());
	const UEdNode_DataGraphNode* NodeB = Cast<UEdNode_DataGraphNode>(B->GetOwningNode());
	for (const UEdGraphPin* TestPin : NodeA->GetOutputPin()->LinkedTo)
	{
		UEdGraphNode* ChildNode = TestPin->GetOwningNode();
		if (UEdNode_DataGraphEdge* EdNode_Edge = Cast<UEdNode_DataGraphEdge>(ChildNode))
		{
			ChildNode = EdNode_Edge->GetEndNode();
		}

		if (ChildNode == NodeB) return false;
	}

	if (NodeA && NodeB)
	{
		Super::TryCreateConnection(NodeA->GetOutputPin(), NodeB->GetInputPin());
		return true;
	}
	
	return false;
}

bool UEdGraphSchema_DataGraph::CreateAutomaticConversionNodeAndConnections(UEdGraphPin* A, UEdGraphPin* B) const
{
	UEdNode_DataGraphNode* NodeA = Cast<UEdNode_DataGraphNode>(A->GetOwningNode());
	UEdNode_DataGraphNode* NodeB = Cast<UEdNode_DataGraphNode>(B->GetOwningNode());
	if (!NodeA || !NodeA->GetOutputPin() || !NodeB || !NodeB->GetInputPin()) return false;

	const UGenericDataGraph* Graph = NodeA->GraphNode->GetGraph();
	const FVector2D InitPos((NodeA->NodePosX + NodeB->NodePosX) / 2, (NodeA->NodePosY + NodeB->NodePosY) / 2);

	FEdSchemaAction_DataGraph_NewEdge Action;
	Action.NodeTemplate = NewObject<UEdNode_DataGraphEdge>(NodeA->GetGraph());
	Action.NodeTemplate->SetEdge(NewObject<UGenericDataEdge>(Action.NodeTemplate, Graph->GetEdgeType()));
	UEdNode_DataGraphEdge* EdgeNode = Cast<UEdNode_DataGraphEdge>(Action.PerformAction(NodeA->GetGraph(), nullptr, InitPos, false));
	EdgeNode->CreateConnections(NodeA, NodeB);

	return true;
}

FConnectionDrawingPolicy* UEdGraphSchema_DataGraph::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	return new FDataGraphConnectionPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

FLinearColor UEdGraphSchema_DataGraph::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor::White;
}

void UEdGraphSchema_DataGraph::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakNodeLinks", "Break Node Links"));
	Super::BreakNodeLinks(TargetNode);
}

void UEdGraphSchema_DataGraph::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotification) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakPinLinks", "Break Pin Links"));
	Super::BreakPinLinks(TargetPin, bSendsNodeNotification);
}

void UEdGraphSchema_DataGraph::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakSinglePinLink", "Break Pin Link"));
	Super::BreakSinglePinLink(SourcePin, TargetPin);
}

UEdGraphPin* UEdGraphSchema_DataGraph::DropPinOnNode(UEdGraphNode* InTargetNode, const FName& InSourcePinName, const FEdGraphPinType& InSourcePinType, const EEdGraphPinDirection InSourcePinDirection) const
{
	const UEdNode_DataGraphNode* EdNode = Cast<UEdNode_DataGraphNode>(InTargetNode);
	switch (InSourcePinDirection)
	{
	case EGPD_Input: return EdNode->GetOutputPin();
	case EGPD_Output: return EdNode->GetInputPin();
	default: return nullptr;
	}
}

bool UEdGraphSchema_DataGraph::SupportsDropPinOnNode(UEdGraphNode* InTargetNode, const FEdGraphPinType& InSourcePinType, EEdGraphPinDirection InSourcePinDirection, FText& OutErrorMessage) const
{
	return Cast<UEdNode_DataGraphNode>(InTargetNode) != nullptr;
}

bool UEdGraphSchema_DataGraph::IsCacheVisualizationOutOfDate(const int32 InVisualizationCacheID) const
{
	return CurrentCacheRefreshID != InVisualizationCacheID;
}

int32 UEdGraphSchema_DataGraph::GetCurrentVisualizationCacheID() const
{
	return CurrentCacheRefreshID;
}

void UEdGraphSchema_DataGraph::ForceVisualizationCacheClear() const
{
	CurrentCacheRefreshID++;
}

#undef LOCTEXT_NAMESPACE
