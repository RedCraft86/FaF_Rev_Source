// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

#include "AutoLayoutStrategy.h"
#include "TreeLayoutStrategy.generated.h"

UCLASS()
class UTreeLayoutStrategy final : public UAutoLayoutStrategy
{
	GENERATED_BODY()
	
public:
	
	UTreeLayoutStrategy() {}
	
	void Layout(UEdGraph* InGraph) override;

private:
	
	void InitPass(UGenericDataNode* RootNode, const FVector2D& Anchor);
	bool ResolveConflictPass(UGenericDataNode* Node);
	bool ResolveConflict(UGenericDataNode* LRoot, UGenericDataNode* RRoot);
	void GetLeftContour(UGenericDataNode* RootNode, int32 Level, TArray<UEdNode_DataGraphNode*>& Contour);
	void GetRightContour(UGenericDataNode* RootNode, int32 Level, TArray<UEdNode_DataGraphNode*>& Contour);
	void ShiftSubTree(UGenericDataNode* RootNode, const FVector2D& Offset);
	void UpdateParentNodePosition(UGenericDataNode* RootNode);
};
