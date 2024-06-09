// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#include "DataGraphAssetEditor.h"
#include "DataGraphAssetToolbar.h"
#include "DataGraphEditorSettings.h"
#include "DataGraphAssetCommands.h"
#include "EdGraphSchema_DataGraph.h"
#include "EdNode_DataGraphEdge.h"
#include "EdNode_DataGraphNode.h"
#include "EdGraph_DataGraph.h"
#include "AutoLayoutStrategy.h"
#include "TreeLayoutStrategy.h"
#include "ForcedDirectLayoutStrategy.h"

#include "GenericDataGraph.h"
#include "DataGraphEditor.h"
#include "EdGraphUtilities.h"
#include "EditorClassUtils.h"
#include "GraphEditorActions.h"
#include "UObject/ObjectSaveContext.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Framework/Commands/GenericCommands.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "DataGraphEditor"

namespace DataGraphStatics
{
	static const FName EditorAppName(TEXT("DataGraphEditorApp"));
	static const FName PropertyID(TEXT("Property"));
	static const FName ViewportID(TEXT("Viewport"));
}

FDataGraphAssetEditor::FDataGraphAssetEditor()
{
	EditingGraph = nullptr;
	OnPackageSavedDelegateHandle = UPackage::PackageSavedWithContextEvent.AddRaw(this, &FDataGraphAssetEditor::OnPackageSavedWithContext);
}

FDataGraphAssetEditor::~FDataGraphAssetEditor()
{
	UPackage::PackageSavedWithContextEvent.Remove(OnPackageSavedDelegateHandle);
}

void FDataGraphAssetEditor::InitEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UGenericDataGraph* Graph)
{
	EditingGraph = Graph;
	CreateEdGraph();

	FGenericCommands::Register();
	FGraphEditorCommands::Register();
	FDataGraphAssetCommands::Register();

	if (!ToolbarBuilder.IsValid())
	{
		ToolbarBuilder = MakeShareable(new FDataGraphAssetToolbar(SharedThis(this)));
	}

	BindCommands();
	CreateInternalWidgets();
	const TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarBuilder->AddDataGraphToolbar(ToolbarExtender);

	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_DataGraphEditor_Layout_v2")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.65f)
					->AddTab(DataGraphStatics::ViewportID, ETabState::OpenedTab)->SetHideTabWell(true)
				)
				->Split
				(
				FTabManager::NewStack()
					->SetSizeCoefficient(0.55f)
					->AddTab(DataGraphStatics::PropertyID, ETabState::OpenedTab)->SetHideTabWell(true)
				)
			)
		);

	InitAssetEditor(Mode, InitToolkitHost, DataGraphStatics::EditorAppName,
		StandaloneDefaultLayout, true, true, EditingGraph);

	RegenerateMenusAndToolbars();
}

void FDataGraphAssetEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu", "Data Graph Editor"));
	const TSharedRef<FWorkspaceItem> WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	
	InTabManager->RegisterTabSpawner(DataGraphStatics::ViewportID, FOnSpawnTab::CreateSP(this, &FDataGraphAssetEditor::SpawnTab_Viewport))
		.SetDisplayName(LOCTEXT("GraphCanvasTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner(DataGraphStatics::PropertyID, FOnSpawnTab::CreateSP(this, &FDataGraphAssetEditor::SpawnTab_Details))
		.SetDisplayName(LOCTEXT("DetailsTab", "Property"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FDataGraphAssetEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(DataGraphStatics::ViewportID);
	InTabManager->UnregisterTabSpawner(DataGraphStatics::PropertyID);
}

void FDataGraphAssetEditor::PostRegenerateMenusAndToolbars()
{
	TArray<UObject*> Objs = GetEditingObjects();
	
	TArray<UClass*> ClassList;
	for (const UObject* Obj : Objs)
	{
		check(Obj);
		ClassList.Add(Obj->GetClass());
	}

	const UClass* CommonBaseClass = UClass::FindCommonBase(ClassList);
	const bool bNotAllSame = Objs.Num() > 0 && Objs[0]->GetClass() != CommonBaseClass;

	if (!CommonBaseClass) return;

	if (CommonBaseClass->ClassGeneratedBy)
	{
		const TSharedRef<SHorizontalBox> MenuOverlayBox = SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.ColorAndOpacity(FSlateColor::UseSubduedForeground())
			.ShadowOffset(FVector2D::UnitVector)
			.Text(NSLOCTEXT("GenericEditor", "BlueprintEditor_ParentClass", "Parent class: "))
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SSpacer)
			.Size(FVector2D(2.0f, 1.0f))
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.ShadowOffset(FVector2D::UnitVector)
			.Text(FText::FromName(CommonBaseClass->ClassGeneratedBy->GetFName()))
			.TextStyle(FAppStyle::Get(), "Common.InheritedFromBlueprintTextStyle")
			.ToolTipText(NSLOCTEXT("GenericEditor", "ParentClassToolTip", "The class that the current Blueprint is based on. The parent provides the base definition, which the current Blueprint extends."))
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.VAlign(VAlign_Center)
			.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
			.OnClicked(this, &FDataGraphAssetEditor::OnFindBlueprintParent, CommonBaseClass->ClassGeneratedBy)
			.ToolTipText(NSLOCTEXT("GenericEditor", "FindParentInCBToolTip", "Find parent in Content Browser"))
			.ContentPadding(4.0f)
			.ForegroundColor(FSlateColor::UseForeground())
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Icons.Search"))
			]
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.VAlign(VAlign_Center)
			.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
			.OnClicked(this, &FDataGraphAssetEditor::OnEditParentClass, CommonBaseClass->ClassGeneratedBy)
			.ToolTipText(NSLOCTEXT("GenericEditor", "EditParentClassToolTip", "Open parent in editor"))
			.ContentPadding(4.0f)
			.ForegroundColor(FSlateColor::UseForeground())
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Icons.Edit"))
			]
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SSpacer)
			.Size(FVector2D(8.0f, 1.0f))
		];
		SetMenuOverlay(MenuOverlayBox);
	}
	else
	{
		const TSharedRef<SHorizontalBox> MenuOverlayBox = SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.ColorAndOpacity(FSlateColor::UseSubduedForeground())
			.ShadowOffset(FVector2D::UnitVector)
			.Text(bNotAllSame ? NSLOCTEXT("GenericEditor", "SimpleAssetEditor_AssetType_Varied", "Common Asset Type: ") : LOCTEXT("SimpleAssetEditor_AssetType", "Asset Type: "))
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(0.0f, 0.0f, 8.0f, 0.0f)
		[
			FEditorClassUtils::GetSourceLink(CommonBaseClass)
		];
		SetMenuOverlay(MenuOverlayBox);
	}
}

FName FDataGraphAssetEditor::GetToolkitFName() const
{
	return FName("FDataGraphEditor");
}

FText FDataGraphAssetEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Data Graph Editor");
}

FText FDataGraphAssetEditor::GetToolkitName() const
{
	return FText::FromString(EditingGraph->GetName());
}

FText FDataGraphAssetEditor::GetToolkitToolTipText() const
{
	return GetToolTipTextForObject(EditingGraph);
}

FLinearColor FDataGraphAssetEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

FString FDataGraphAssetEditor::GetWorldCentricTabPrefix() const
{
	return TEXT("DataGraphEditor");
}

FString FDataGraphAssetEditor::GetDocumentationLink() const
{
	return TEXT("https://github.com/jinyuliao/GenericGraph");
}

void FDataGraphAssetEditor::SaveAsset_Execute()
{
	if (EditingGraph)
	{
		RebuildDataGraph();
	}

	FAssetEditorToolkit::SaveAsset_Execute();
}

void FDataGraphAssetEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EditingGraph);
	Collector.AddReferencedObject(EditingGraph->EdGraph);
}

void FDataGraphAssetEditor::RegisterToolbarTab(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}

TSharedRef<SDockTab> FDataGraphAssetEditor::SpawnTab_Viewport(const FSpawnTabArgs& Args) const
{
	check(Args.GetTabId() == DataGraphStatics::ViewportID);
	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab).Label(LOCTEXT("ViewportLabel", "Viewport"));
	if (ViewportWidget.IsValid()) SpawnedTab->SetContent(ViewportWidget.ToSharedRef());
	return SpawnedTab;
}

TSharedRef<SDockTab> FDataGraphAssetEditor::SpawnTab_Details(const FSpawnTabArgs& Args) const
{
	check(Args.GetTabId() == DataGraphStatics::PropertyID);
	return SNew(SDockTab).Label(LOCTEXT("DetailsLabel", "Property"))[ PropertyWidget.ToSharedRef() ];
}

void FDataGraphAssetEditor::CreateInternalWidgets()
{
	ViewportWidget = CreateViewportWidget();

	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.NotifyHook = this;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyWidget = PropertyModule.CreateDetailView(Args);
	PropertyWidget->SetObject(EditingGraph);
	PropertyWidget->OnFinishedChangingProperties().AddSP(this,&FDataGraphAssetEditor::OnFinishedChangingProperties);
}

TSharedRef<SGraphEditor> FDataGraphAssetEditor::CreateViewportWidget()
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText_DataGraph", "Data Graph");

	CreateCommandList();

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FDataGraphAssetEditor::OnSelectedNodesChanged);

	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.GraphToEdit(EditingGraph->EdGraph)
		.GraphEvents(InEvents)
		.AutoExpandActionMenu(true)
		.ShowGraphStateOverlay(false);
}

void FDataGraphAssetEditor::BindCommands()
{
	 ToolkitCommands->MapAction(FDataGraphAssetCommands::Get().AutoArrange,
	 	FExecuteAction::CreateSP(this, &FDataGraphAssetEditor::AutoArrange),
	 	FCanExecuteAction::CreateSP(this, &FDataGraphAssetEditor::CanAutoArrange)
	);
}

void FDataGraphAssetEditor::CreateEdGraph() const
{
	if (!EditingGraph->EdGraph)
	{
		EditingGraph->EdGraph = CastChecked<UEdGraph_DataGraph>(FBlueprintEditorUtils::CreateNewGraph(EditingGraph,
			NAME_None, UEdGraph_DataGraph::StaticClass(), UEdGraphSchema_DataGraph::StaticClass()));

		EditingGraph->EdGraph->bAllowDeletion = false;
		const UEdGraphSchema* Schema = EditingGraph->EdGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*EditingGraph->EdGraph);
	}
}

void FDataGraphAssetEditor::CreateCommandList()
{
	if (GraphEditorCommands.IsValid()) return;
	GraphEditorCommands = MakeShareable(new FUICommandList);

	GraphEditorCommands->MapAction(FDataGraphAssetCommands::Get().AutoArrange,
		FExecuteAction::CreateRaw(this, &FDataGraphAssetEditor::AutoArrange),
		FCanExecuteAction::CreateRaw(this, &FDataGraphAssetEditor::CanAutoArrange)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
		FExecuteAction::CreateRaw(this, &FDataGraphAssetEditor::SelectAllNodes),
		FCanExecuteAction::CreateRaw(this, &FDataGraphAssetEditor::CanSelectAllNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FDataGraphAssetEditor::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FDataGraphAssetEditor::CanDeleteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FDataGraphAssetEditor::CopySelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FDataGraphAssetEditor::CanCopyNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FDataGraphAssetEditor::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FDataGraphAssetEditor::CanCutNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FDataGraphAssetEditor::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FDataGraphAssetEditor::CanPasteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FDataGraphAssetEditor::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FDataGraphAssetEditor::CanDuplicateNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Rename,
		FExecuteAction::CreateSP(this, &FDataGraphAssetEditor::OnRenameNode),
		FCanExecuteAction::CreateSP(this, &FDataGraphAssetEditor::CanRenameNodes)
	);
}

void FDataGraphAssetEditor::RebuildDataGraph() const
{
	if (EditingGraph == nullptr)
	{
		UE_LOG(DataGraphEditor, Warning, TEXT("FDataGraphAssetEditor::RebuildDataGraph EditingGraph is nullptr"));
		return;
	}

	UEdGraph_DataGraph* EdGraph = Cast<UEdGraph_DataGraph>(EditingGraph->EdGraph);
	check(EdGraph);
	EdGraph->RebuildDataGraph();
}

TSharedPtr<SGraphEditor> FDataGraphAssetEditor::GetCurrGraphEditor() const
{
	return ViewportWidget;
}

FGraphPanelSelectionSet FDataGraphAssetEditor::GetSelectedNodes() const
{
	return GetCurrGraphEditor().IsValid() ? GetCurrGraphEditor()->GetSelectedNodes() : FGraphPanelSelectionSet{};
}

void FDataGraphAssetEditor::SelectAllNodes() const
{
	if (GetCurrGraphEditor().IsValid()) GetCurrGraphEditor()->SelectAllNodes();
}

bool FDataGraphAssetEditor::CanSelectAllNodes() const
{
	return true;
}

void FDataGraphAssetEditor::DeleteSelectedNodes() const
{
	const TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FScopedTransaction Transaction(FGenericCommands::Get().Delete->GetDescription());

	CurrentGraphEditor->GetCurrentGraph()->Modify();

	const FGraphPanelSelectionSet SelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UEdGraphNode* EdNode = Cast<UEdGraphNode>(*NodeIt);
		if (EdNode == nullptr || !EdNode->CanUserDeleteNode())
			continue;
		
		if (UEdNode_DataGraphNode* EdNode_Node = Cast<UEdNode_DataGraphNode>(EdNode))
		{
			EdNode_Node->Modify();
			if (const UEdGraphSchema* Schema = EdNode_Node->GetSchema()) Schema->BreakNodeLinks(*EdNode_Node);
			EdNode_Node->DestroyNode();
		}
		else
		{
			EdNode->Modify();
			EdNode->DestroyNode();
		}
	}
}

bool FDataGraphAssetEditor::CanDeleteNodes() const
{
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		const UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanUserDeleteNode())
		{
			return true;
		}
	}

	return false;
}

void FDataGraphAssetEditor::DeleteSelectedDuplicateNodes() const
{
	const TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FGraphPanelSelectionSet OldSelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}

	DeleteSelectedNodes();

	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}
}

void FDataGraphAssetEditor::CutSelectedNodes() const
{
	CopySelectedNodes();
	DeleteSelectedDuplicateNodes();
}

bool FDataGraphAssetEditor::CanCutNodes() const
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FDataGraphAssetEditor::CopySelectedNodes() const
{
	FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	FString ExportedText;

	for (FGraphPanelSelectionSet::TIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node == nullptr)
		{
			SelectedIter.RemoveCurrent();
			continue;
		}

		if (UEdNode_DataGraphEdge* EdNode_Edge = Cast<UEdNode_DataGraphEdge>(*SelectedIter))
		{
			const UEdNode_DataGraphNode* StartNode = EdNode_Edge->GetStartNode();
			const UEdNode_DataGraphNode* EndNode = EdNode_Edge->GetEndNode();
			if (!SelectedNodes.Contains(StartNode) || !SelectedNodes.Contains(EndNode))
			{
				SelectedIter.RemoveCurrent();
				continue;
			}
		}

		Node->PrepareForCopying();
	}

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool FDataGraphAssetEditor::CanCopyNodes() const
{
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		const UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			return true;
		}
	}

	return false;
}

void FDataGraphAssetEditor::PasteNodes() const
{
	if (GetCurrGraphEditor().IsValid()) PasteNodesHere(GetCurrGraphEditor()->GetPasteLocation());
}

void FDataGraphAssetEditor::PasteNodesHere(const FVector2D& Location) const
{
	const TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (!CurrentGraphEditor.IsValid()) return;

	UEdGraph* EdGraph = CurrentGraphEditor->GetCurrentGraph();
	{
		const FScopedTransaction Transaction(FGenericCommands::Get().Paste->GetDescription());
		EdGraph->Modify();

		CurrentGraphEditor->ClearSelectionSet();

		FString TextToImport;
		FPlatformApplicationMisc::ClipboardPaste(TextToImport);

		TSet<UEdGraphNode*> PastedNodes;
		FEdGraphUtilities::ImportNodesFromText(EdGraph, TextToImport, PastedNodes);

		FVector2D AvgNodePosition(0.0f, 0.0f);
		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			const UEdGraphNode* Node = *It;
			AvgNodePosition.X += Node->NodePosX;
			AvgNodePosition.Y += Node->NodePosY;
		}

		const float InvNumNodes = 1.0f / (float)PastedNodes.Num();
		AvgNodePosition.X *= InvNumNodes;
		AvgNodePosition.Y *= InvNumNodes;

		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			UEdGraphNode* Node = *It;
			CurrentGraphEditor->SetNodeSelection(Node, true);
			Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X;
			Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y;
			Node->SnapToGrid(16);
			Node->CreateNewGuid();
		}
	}

	CurrentGraphEditor->NotifyGraphChanged();
	if (UObject* GraphOwner = EdGraph->GetOuter())
	{
		GraphOwner->PostEditChange();
		GraphOwner->MarkPackageDirty();
	}
}

bool FDataGraphAssetEditor::CanPasteNodes() const
{
	const TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (!CurrentGraphEditor.IsValid()) return false;

	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);
	return FEdGraphUtilities::CanImportNodesFromText(CurrentGraphEditor->GetCurrentGraph(), ClipboardContent);
}

void FDataGraphAssetEditor::DuplicateNodes() const
{
	CopySelectedNodes();
	PasteNodes();
}

bool FDataGraphAssetEditor::CanDuplicateNodes() const
{
	return CanCopyNodes();
}

void FDataGraphAssetEditor::GraphSettings() const
{
	PropertyWidget->SetObject(EditingGraph);
}

bool FDataGraphAssetEditor::CanGraphSettings() const
{
	return true;
}

void FDataGraphAssetEditor::AutoArrange() const
{
	UEdGraph_DataGraph* EdGraph = Cast<UEdGraph_DataGraph>(EditingGraph->EdGraph);
	check(EdGraph);
	
	const FScopedTransaction Transaction(LOCTEXT("DataGraphEditorAutoArrange", "Data Graph Editor: Auto Arrange"));
	
	EdGraph->Modify();
	
	UAutoLayoutStrategy* LayoutStrategy = nullptr;
	switch (UDataGraphEditorSettings::Get()->AutoLayoutStrategy)
	{
	case EDataGraphLayoutStrategy::Tree:
		LayoutStrategy = NewObject<UAutoLayoutStrategy>(EdGraph, UTreeLayoutStrategy::StaticClass());
		break;
	case EDataGraphLayoutStrategy::ForceDirected:
		LayoutStrategy = NewObject<UAutoLayoutStrategy>(EdGraph, UForcedDirectLayoutStrategy::StaticClass());
		break;
	default: break;
	}

	if (LayoutStrategy)
	{
		LayoutStrategy->Layout(EdGraph);
		LayoutStrategy->ConditionalBeginDestroy();
	}
	else
	{
		UE_LOG(DataGraphEditor, Error, TEXT("FDataGraphAssetEditor::AutoArrange LayoutStrategy is null."));
	}
}

bool FDataGraphAssetEditor::CanAutoArrange() const
{
	return EditingGraph && Cast<UEdGraph_DataGraph>(EditingGraph->EdGraph);
}

void FDataGraphAssetEditor::OnRenameNode() const
{
	if (GetCurrGraphEditor().IsValid())
	{
		const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
		for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
		{
			const UEdGraphNode* SelectedNode = Cast<UEdGraphNode>(*NodeIt);
			if (SelectedNode && SelectedNode->bCanRenameNode)
			{
				GetCurrGraphEditor()->IsNodeTitleVisible(SelectedNode, true);
				break;
			}
		}
	}
}

bool FDataGraphAssetEditor::CanRenameNodes() const
{
	const UEdGraph_DataGraph* EdGraph = Cast<UEdGraph_DataGraph>(EditingGraph->EdGraph);
	check(EdGraph);

	const UGenericDataGraph* Graph = EdGraph->GetGraph();
	check(Graph)
	
	return GetSelectedNodes().Num() == 1;
}

void FDataGraphAssetEditor::OnSelectedNodesChanged(const TSet<UObject*>& NewSelection)
{
	TArray<UObject*> Selection;
	for (UObject* SelectionEntry : NewSelection)
	{
		Selection.Add(SelectionEntry);
	}

	if (Selection.IsEmpty()) 
	{
		PropertyWidget->SetObject(EditingGraph);

	}
	else
	{
		PropertyWidget->SetObjects(Selection);
	}
}

void FDataGraphAssetEditor::OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (EditingGraph) EditingGraph->EdGraph->GetSchema()->ForceVisualizationCacheClear();
}

void FDataGraphAssetEditor::OnPackageSavedWithContext(const FString& PackageFileName, UPackage* Package, FObjectPostSaveContext ObjectSaveContext)
{
	RebuildDataGraph();
}

FReply FDataGraphAssetEditor::OnFindBlueprintParent(TObjectPtr<UObject> SyncToClass) const
{
	if (SyncToClass)
	{
		GEditor->SyncBrowserToObjects({SyncToClass});
	}

	return FReply::Handled();
}

FReply FDataGraphAssetEditor::OnEditParentClass(TObjectPtr<UObject> EditClass) const
{
	if (EditClass)
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(EditClass);
	}

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
