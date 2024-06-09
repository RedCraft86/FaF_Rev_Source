// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

#include "SGraphNode.h"

class SNode_DataGraphEdge final : public SGraphNode
{
public:
	
	SLATE_BEGIN_ARGS(SNode_DataGraphEdge){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, class UEdNode_DataGraphEdge* InNode);
	
	virtual void UpdateGraphNode() override;
	virtual bool RequiresSecondPassLayout() const override;
	virtual void PerformSecondPassLayout(const TMap< UObject*, TSharedRef<SNode> >& NodeToWidgetLookup) const override;
	
	void PositionBetweenTwoNodesWithOffset(const FGeometry& StartGeom, const FGeometry& EndGeom, int32 NodeIndex, int32 MaxNodes) const;
	void OnNameTextCommitted(const FText& InText, ETextCommit::Type CommitInfo);

private:
	
	FSlateColor GetEdgeColor() const;
	const FSlateBrush* GetEdgeImage() const;
	EVisibility GetEdgeImageVisibility() const;
	EVisibility GetEdgeTitleVisibility() const;

	TSharedPtr<STextEntryPopup> TextEntryWidget;
};
