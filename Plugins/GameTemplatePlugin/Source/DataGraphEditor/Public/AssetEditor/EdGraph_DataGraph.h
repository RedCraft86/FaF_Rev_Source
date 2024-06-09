// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

#include "EdGraph/EdGraph.h"
#include "EdGraph_DataGraph.generated.h"

UCLASS()
class UEdGraph_DataGraph final : public UEdGraph
{
	GENERATED_BODY()

public:
	
	UEdGraph_DataGraph() {}

	UPROPERTY(Transient) TMap<TObjectPtr<class UGenericDataNode>, TObjectPtr<class UEdNode_DataGraphNode>> NodeMap;
	UPROPERTY(Transient) TMap<TObjectPtr<class UGenericDataEdge>, TObjectPtr<class UEdNode_DataGraphEdge>> EdgeMap;


	void RebuildDataGraph();
	class UGenericDataGraph* GetGraph() const;

	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	virtual void PostEditUndo() override;
	
private:
	
	void Clear();
	void SortNodes(UGenericDataNode* RootNode);
};
