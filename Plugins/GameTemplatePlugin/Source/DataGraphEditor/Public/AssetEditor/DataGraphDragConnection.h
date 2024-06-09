// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

#include "GraphEditorDragDropAction.h"

struct FGraphPinHandle;

class FDataGraphDragConnection final : public FGraphEditorDragDropAction
{
public:

	DRAG_DROP_OPERATOR_TYPE(FDragConnection, FGraphEditorDragDropAction)
	
	static TSharedRef<FDataGraphDragConnection> New(const TSharedRef<SGraphPanel>& InGraphPanel, const TArray<FGraphPinHandle>& InStartingPins);

	// FDragDropOperation interface
	virtual void OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent) override;
	// End of FDragDropOperation interface

	// FGraphEditorDragDropAction interface
	virtual void HoverTargetChanged() override;
	virtual FReply DroppedOnPin(FVector2D ScreenPosition, FVector2D GraphPosition) override;
	virtual FReply DroppedOnNode(FVector2D ScreenPosition, FVector2D GraphPosition) override;
	virtual FReply DroppedOnPanel(const TSharedRef< SWidget >& Panel, FVector2D ScreenPosition, FVector2D GraphPosition, UEdGraph& Graph) override;
	virtual void OnDragged(const FDragDropEvent& DragDropEvent) override;
	// End of FGraphEditorDragDropAction interface

	void ValidateGraphPinList(TArray<UEdGraphPin*>& OutValidPins);

private:

	FDataGraphDragConnection(const TSharedRef<SGraphPanel>& InGraphPanel, const TArray<FGraphPinHandle>& DraggedPins);

	TSharedPtr<SGraphPanel> GraphPanel;
	TArray<FGraphPinHandle> DraggingPins;
	FVector2D DecoratorAdjust;
};
