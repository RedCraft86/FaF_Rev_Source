// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "EdNode_DataGraphEdge.generated.h"

class UGenericDataEdge;
class UEdNode_DataGraphNode;

UCLASS()
class UEdNode_DataGraphEdge final : public UEdGraphNode
{
	GENERATED_BODY()

public:

	UEdNode_DataGraphEdge();

	UPROPERTY()
		TObjectPtr<UEdGraph> Graph;

	UPROPERTY(VisibleAnywhere, Instanced, Category = "DataGraph")
		TObjectPtr<UGenericDataEdge> GraphEdge;

	void SetEdge(UGenericDataEdge* Edge);
	void CreateConnections(UEdNode_DataGraphNode* Start, UEdNode_DataGraphNode* End);
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void PrepareForCopying() override;
	UEdNode_DataGraphNode* GetStartNode();
	UEdNode_DataGraphNode* GetEndNode();
	UEdGraphPin* GetInputPin() const { return Pins[0]; }
	UEdGraphPin* GetOutputPin() const { return Pins[1]; }
};
