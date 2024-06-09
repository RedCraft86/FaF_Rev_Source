// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

#include "SGraphNode.h"

class SNode_DataGraphNode final : public SGraphNode
{
public:
	
	SLATE_BEGIN_ARGS(SNode_DataGraphNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, class UEdNode_DataGraphNode* InNode);

	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual bool IsNameReadOnly() const override;

	const FSlateBrush* GetNameIcon() const;
	FSlateColor GetBackgroundColor() const;
	FSlateColor GetBorderBackgroundColor() const;
	EVisibility GetDragOverMarkerVisibility() const;
	void OnNameTextCommitted(const FText& InText, ETextCommit::Type CommitInfo);
};
