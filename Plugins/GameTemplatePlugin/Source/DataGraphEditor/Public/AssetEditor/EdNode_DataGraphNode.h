// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "EdNode_DataGraphNode.generated.h"

class UGenericDataNode;
class UEdGraph_DataGraph;
class SNode_DataGraphNode;

UCLASS()
class UEdNode_DataGraphNode final : public UEdGraphNode
{
	GENERATED_BODY()

public:
	
	UEdNode_DataGraphNode();

	SNode_DataGraphNode* SNode;
	
	UPROPERTY(VisibleAnywhere, Instanced, Category = "DataGraph")
		TObjectPtr<UGenericDataNode> GraphNode;

	void SetDataGraphNode(UGenericDataNode* InNode);
	UEdGraph_DataGraph* GetDataGraphEdGraph() const;

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PrepareForCopying() override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	FLinearColor GetBackgroundColor() const;
	UEdGraphPin* GetInputPin() const { return Pins[0]; }
	UEdGraphPin* GetOutputPin() const { return Pins[1]; }
};
