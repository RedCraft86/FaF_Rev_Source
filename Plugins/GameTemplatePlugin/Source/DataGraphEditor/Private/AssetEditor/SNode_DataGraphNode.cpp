// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#include "SNode_DataGraphNode.h"
#include "DataGraphDragConnection.h"
#include "EdNode_DataGraphNode.h"
#include "DataGraphColors.h"

#include "SGraphPin.h"
#include "SCommentBubble.h"
#include "GraphEditorSettings.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "GenericDataNode.h"

#define LOCTEXT_NAMESPACE "DataGraphEditor"

class SDataGraphPin final : public SGraphPin
{
public:
	
	SLATE_BEGIN_ARGS(SDataGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		SetCursor(EMouseCursor::Default);
		bShowLabel = true;
		GraphPinObj = InPin;
		check(GraphPinObj);
		const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
		check(Schema);
		SBorder::Construct(SBorder::FArguments()
			.BorderImage(this, &SDataGraphPin::GetPinBorder)
			.BorderBackgroundColor(this, &SDataGraphPin::GetPinColor)
			.OnMouseButtonDown(this, &SDataGraphPin::OnPinMouseDown)
			.Cursor(this, &SDataGraphPin::GetPinCursor)
			.Padding(FMargin(5.0f))
		);
	}

protected:
	
	virtual FSlateColor GetPinColor() const override
	{
		return DataGraphColors::Pin::Default;
	}

	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override
	{
		return SNew(STextBlock);
	}

	const FSlateBrush* GetBorder() const
	{
		return FAppStyle::GetBrush(TEXT("Graph.StateNode.Body"));
	}

	virtual TSharedRef<FDragDropOperation> SpawnPinDragEvent(const TSharedRef<SGraphPanel>& InGraphPanel, const TArray< TSharedRef<SGraphPin> >& InStartingPins) override
	{
		TArray<FGraphPinHandle> PinHandles;
		PinHandles.Reserve(InStartingPins.Num());
		for (const TSharedRef<SGraphPin>& PinWidget : InStartingPins)
		{
			PinHandles.Add(PinWidget->GetPinObj());
		}

		return FDataGraphDragConnection::New(InGraphPanel, PinHandles);
	}

};

void SNode_DataGraphNode::Construct(const FArguments& InArgs, UEdNode_DataGraphNode* InNode)
{
	GraphNode = InNode;
	UpdateGraphNode();
	InNode->SNode = this;
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SNode_DataGraphNode::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	TSharedPtr<SErrorText> ErrorText;
	TSharedPtr<SVerticalBox> NodeBody;
	const FSlateBrush *NodeTypeIcon = GetNameIcon();
	constexpr FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);
	const TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	ContentScale.Bind(this, &SGraphNode::GetContentScale);
	GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0.0f)
			.BorderBackgroundColor(this, &SNode_DataGraphNode::GetBorderBackgroundColor)
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.FillHeight(1)
					[
						SAssignNew(LeftNodeBox, SVerticalBox)
					]
					+ SVerticalBox::Slot()
					.FillHeight(1)
					[
						SAssignNew(RightNodeBox, SVerticalBox)
					]
				]
				+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(8.0f)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("Graph.StateNode.ColorSpill"))
					.BorderBackgroundColor(TitleShadowColor)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Visibility(EVisibility::SelfHitTestInvisible)
					.Padding(6.0f)
					[
						SAssignNew(NodeBody, SVerticalBox)
						.Visibility(EVisibility::Visible)
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.AutoWidth()
							[
								SAssignNew(ErrorText, SErrorText)
								.BackgroundColor(this, &SNode_DataGraphNode::GetErrorColor)
								.ToolTipText(this, &SNode_DataGraphNode::GetErrorMsgToolTip)
							]
							+SHorizontalBox::Slot()
							.AutoWidth()
							.VAlign(VAlign_Center)
							[
								SNew(SImage)
								.Image(NodeTypeIcon)
							]
							+ SHorizontalBox::Slot()
							.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
							[
								SNew(SVerticalBox)
								+ SVerticalBox::Slot()
								.AutoHeight()
								[
									SAssignNew(InlineEditableText, SInlineEditableTextBlock)
									.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
									.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
									.OnVerifyTextChanged(this, &SNode_DataGraphNode::OnVerifyNameTextChanged)
									.OnTextCommitted(this, &SNode_DataGraphNode::OnNameTextCommitted)
									.IsReadOnly(this, &SNode_DataGraphNode::IsNameReadOnly)
									.IsSelected(this, &SNode_DataGraphNode::IsSelectedExclusively)
								]
								+ SVerticalBox::Slot()
								.AutoHeight()
								[
									NodeTitle.ToSharedRef()
								]
							]
						]					
					]
				]
			]
		];

	TSharedPtr<SCommentBubble> CommentBubble;
	const FSlateColor CommentColor = GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor;
	
	SAssignNew(CommentBubble, SCommentBubble)
		.GraphNode(GraphNode)
		.Text(this, &SGraphNode::GetNodeComment)
		.OnTextCommitted(this, &SGraphNode::OnCommentTextCommitted)
		.ColorAndOpacity(CommentColor)
		.AllowPinning(true)
		.EnableTitleBarBubble(true)
		.EnableBubbleCtrls(true)
		.GraphLOD(this, &SGraphNode::GetCurrentLOD)
		.IsGraphNodeHovered(this, &SGraphNode::IsHovered);
	
	GetOrAddSlot(ENodeZone::TopCenter)
		.SlotOffset(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetOffset))
		.SlotSize(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetSize))
		.AllowScaling(TAttribute<bool>(CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
		.VAlign(VAlign_Top)
		[
			CommentBubble.ToSharedRef()
		];

	ErrorReporting = ErrorText;
	ErrorReporting->SetError(ErrorMsg);
	CreatePinWidgets();
}

void SNode_DataGraphNode::CreatePinWidgets()
{
	UEdNode_DataGraphNode* StateNode = CastChecked<UEdNode_DataGraphNode>(GraphNode);
	for (int32 PinIdx = 0; PinIdx < StateNode->Pins.Num(); PinIdx++)
	{
		UEdGraphPin* MyPin = StateNode->Pins[PinIdx];
		if (!MyPin->bHidden)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SDataGraphPin, MyPin);
			AddPin(NewPin.ToSharedRef());
		}
	}
}

void SNode_DataGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));
	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	if (PinObj && PinObj->bAdvancedView)
	{
		PinToAdd->SetVisibility(TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced));
	}

	TSharedPtr<SVerticalBox> PinBox;
	if (PinToAdd->GetDirection() == EGPD_Input)
	{
		PinBox = LeftNodeBox;
		InputPins.Add(PinToAdd);
	}
	else
	{
		PinBox = RightNodeBox;
		OutputPins.Add(PinToAdd);
	}

	if (PinBox)
	{
		PinBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillHeight(1.0f)
			[
				PinToAdd
			];
	}
}

bool SNode_DataGraphNode::IsNameReadOnly() const
{
	const UEdNode_DataGraphNode* Node = Cast<UEdNode_DataGraphNode>(GraphNode);
	check(Node);
	
	return !Node->GraphNode->CanEditName() || SGraphNode::IsNameReadOnly();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

const FSlateBrush* SNode_DataGraphNode::GetNameIcon() const
{
	return FAppStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Icon"));
}

FSlateColor SNode_DataGraphNode::GetBackgroundColor() const
{
	const UEdNode_DataGraphNode* Node = CastChecked<UEdNode_DataGraphNode>(GraphNode);
	return Node ? Node->GetBackgroundColor() : DataGraphColors::NodeBorder::HighlightAbortRange0;
}

FSlateColor SNode_DataGraphNode::GetBorderBackgroundColor() const
{
	return DataGraphColors::NodeBody::Default;
}

EVisibility SNode_DataGraphNode::GetDragOverMarkerVisibility() const
{
	return EVisibility::Visible;
}

void SNode_DataGraphNode::OnNameTextCommitted(const FText& InText, ETextCommit::Type CommitInfo)
{
	SGraphNode::OnNameTextCommited(InText, CommitInfo);
	UEdNode_DataGraphNode* Node = CastChecked<UEdNode_DataGraphNode>(GraphNode);
	if (Node && Node->GraphNode)
	{
		const FScopedTransaction Transaction(LOCTEXT("RenameNode", "Data Graph Editor: Rename Node"));
		Node->Modify();
		Node->GraphNode->Modify();
		Node->GraphNode->SetNodeTitle(InText);
		UpdateGraphNode();
	}
}

#undef LOCTEXT_NAMESPACE
