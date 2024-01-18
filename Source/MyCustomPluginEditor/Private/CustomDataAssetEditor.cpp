#include "CustomDataAssetEditor.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "CustomDataAsset.h"
#include "Framework/Commands/GenericCommands.h"
#include "EdGraphUtilities.h"
#include "HAL/PlatformApplicationMisc.h"
#include "AssetEditor/EdCustomGraphNode.h"
#include "AssetEditor/EdCustomGraphSchema.h"
#include <AssetEditor/EdCustomGraph.h>

#define LOCTEXT_NAMESPACE "CustomDataAssetEditor"

const FName CustomDataAssetEditorAppName = FName(TEXT("CustomDataAssetEditorApp"));

struct FCustomDataAssetAssetEditorTabs
{
	static const FName GraphViewId;
	static const FName NodeListID;
	static const FName CustomDataAssetPropertyID;	
};

// Tab IDs
const FName FCustomDataAssetAssetEditorTabs::GraphViewId(TEXT("RuleList"));
const FName FCustomDataAssetAssetEditorTabs::NodeListID(TEXT("NodeList"));
const FName FCustomDataAssetAssetEditorTabs::CustomDataAssetPropertyID(TEXT("CustomDataAssetPropertyID"));

void FCustomDataAssetEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_CustomDataAssetEditor", "Custom Data Asset Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FCustomDataAssetAssetEditorTabs::GraphViewId, FOnSpawnTab::CreateSP(this, &FCustomDataAssetEditor::SpawnTab_RuleList))
		.SetDisplayName(LOCTEXT("DetailsTab", "Graph"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(FCustomDataAssetAssetEditorTabs::NodeListID, FOnSpawnTab::CreateSP(this, &FCustomDataAssetEditor::SpawnTab_NodeList))
		.SetDisplayName(LOCTEXT("NodeListTab", "Node List"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(FCustomDataAssetAssetEditorTabs::CustomDataAssetPropertyID, FOnSpawnTab::CreateSP(this, &FCustomDataAssetEditor::SpawnTab_Details))
		.SetDisplayName(LOCTEXT("DetailsTab", "Property"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
		
}

void FCustomDataAssetEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(FCustomDataAssetAssetEditorTabs::GraphViewId);
	InTabManager->UnregisterTabSpawner(FCustomDataAssetAssetEditorTabs::NodeListID);
	InTabManager->UnregisterTabSpawner(FCustomDataAssetAssetEditorTabs::CustomDataAssetPropertyID);
}

void FCustomDataAssetEditor::InitCustomDataAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UCustomDataAsset* InCustomDataAsset)
{
    CustomDataAsset = InCustomDataAsset;
	CreateEdGraph();

	FGenericCommands::Register();

	CreateInternalWidgets();

	// Layout of the tabs in the editor window
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_CustomDataAssetEditor_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
#if ENGINE_MAJOR_VERSION < 5
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)->SetHideTabWell(true)
			)
#endif // #if ENGINE_MAJOR_VERSION < 5
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->Split(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.5f)
						->AddTab(FCustomDataAssetAssetEditorTabs::GraphViewId, ETabState::OpenedTab)->SetHideTabWell(true)
					)
					->Split(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.5f)
						->AddTab(FCustomDataAssetAssetEditorTabs::NodeListID, ETabState::OpenedTab)
					)
				)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(1.0f)
						->AddTab(FCustomDataAssetAssetEditorTabs::CustomDataAssetPropertyID, ETabState::OpenedTab)
					)
				)
			)
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
    FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, CustomDataAssetEditorAppName, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, CustomDataAsset);
}

FName FCustomDataAssetEditor::GetToolkitFName() const
{
    return FName("CustomDataAssetEditor");
}

FText FCustomDataAssetEditor::GetBaseToolkitName() const
{
    return LOCTEXT("CustomDataAssetEditorAppLabel", "Custom Data Asset Editor");
}

FText FCustomDataAssetEditor::GetToolkitName() const
{
    return FText::FromString(CustomDataAsset->GetName());
}

FLinearColor FCustomDataAssetEditor::GetWorldCentricTabColorScale() const
{
    return FLinearColor::White;
}

FString FCustomDataAssetEditor::GetWorldCentricTabPrefix() const
{
	return TEXT("CustomDataAssetEditor");
}

void FCustomDataAssetEditor::SaveAsset_Execute()
{
	RebuildCustomData();
	FAssetEditorToolkit::SaveAsset_Execute();
}

TSharedRef<SDockTab> FCustomDataAssetEditor::SpawnTab_RuleList(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FCustomDataAssetAssetEditorTabs::GraphViewId);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("RuleListTab_Title", "RuleList"));

	if (CustomGraphWidget.IsValid())
	{
		SpawnedTab->SetContent(CustomGraphWidget.ToSharedRef());
	}

	return SpawnedTab;
}

void FCustomDataAssetEditor::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	TArray<UObject*> Selection;

	for (UObject* SelectionEntry : NewSelection)
	{
		Selection.Add(SelectionEntry);
	}

	if (Selection.Num() == 0)
	{
		PropertyWidget->SetObject(CustomDataAsset);
	}
	else
	{
		PropertyWidget->SetObjects(Selection);
	}
}

TSharedRef<SGraphEditor> FCustomDataAssetEditor::CreateGraphtWidget()
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText_CustomGraph", "Custom Graph");

	CreateCommandList();

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FCustomDataAssetEditor::OnSelectedNodesChanged);

	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.GraphToEdit(CustomDataAsset->EdGraph)
		.GraphEvents(InEvents)
		.AutoExpandActionMenu(true)
		.ShowGraphStateOverlay(false);
}

TSharedRef<SDockTab> FCustomDataAssetEditor::SpawnTab_Details(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FCustomDataAssetAssetEditorTabs::CustomDataAssetPropertyID);

	return SNew(SDockTab)
#if ENGINE_MAJOR_VERSION < 5
		.Icon(FAppStyle::GetBrush("LevelEditor.Tabs.Details"))
#endif // #if ENGINE_MAJOR_VERSION < 5
		.Label(LOCTEXT("Details_Title", "Property"))
		[
			PropertyWidget.ToSharedRef()
		];
}

TSharedRef<SDockTab> FCustomDataAssetEditor::SpawnTab_NodeList(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FCustomDataAssetAssetEditorTabs::NodeListID);

	return SNew(SDockTab)
#if ENGINE_MAJOR_VERSION < 5
		.Icon(FAppStyle::GetBrush("LevelEditor.Tabs.Details"))
#endif // #if ENGINE_MAJOR_VERSION < 5
		.Label(LOCTEXT("Details_Title", "Property"))
		[
			CustomNodeEditorWidget.ToSharedRef()
		];
}

void FCustomDataAssetEditor::CreateInternalWidgets()
{
	CustomGraphWidget = CreateGraphtWidget();

	CustomNodeEditorWidget = SNew(SCustomNodeEditorWidget)
		.CustomDataAsset(CustomDataAsset)
		.CustomGraphWidget(CustomGraphWidget.Get())
		.PropertyWidget(PropertyWidget.Get());

	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyWidget = PropertyModule.CreateDetailView(Args);
	PropertyWidget->SetObject(CustomDataAsset);

	EditorSettingsWidget = PropertyModule.CreateDetailView(Args);
}

void FCustomDataAssetEditor::CreateEdGraph()
{
	if (CustomDataAsset && CustomDataAsset->EdGraph == nullptr)
	{
		CustomDataAsset->EdGraph = CastChecked<UEdCustomGraph>(FBlueprintEditorUtils::CreateNewGraph(CustomDataAsset, NAME_None, UEdCustomGraph::StaticClass(), UEdCustomGraphSchema::StaticClass()));
		CustomDataAsset->EdGraph->bAllowDeletion = false;

		// Give the schema a chance to fill out any required nodes (like the results node)
		const UEdGraphSchema* Schema = CustomDataAsset->EdGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*CustomDataAsset->EdGraph);
	}
}

void FCustomDataAssetEditor::CreateCommandList()
{
	if (GraphEditorCommands.IsValid())
	{
		return;
	}

	GraphEditorCommands = MakeShareable(new FUICommandList);

	// Can't use CreateSP here because derived editor are already implementing TSharedFromThis<FAssetEditorToolkit>
	// however it should be safe, since commands are being used only within this editor
	// if it ever crashes, this function will have to go away and be reimplemented in each derived class

	GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
		FExecuteAction::CreateRaw(this, &FCustomDataAssetEditor::SelectAllNodes),
		FCanExecuteAction::CreateRaw(this, &FCustomDataAssetEditor::CanSelectAllNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FCustomDataAssetEditor::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FCustomDataAssetEditor::CanDeleteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FCustomDataAssetEditor::CopySelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FCustomDataAssetEditor::CanCopyNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FCustomDataAssetEditor::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FCustomDataAssetEditor::CanCutNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FCustomDataAssetEditor::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FCustomDataAssetEditor::CanPasteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FCustomDataAssetEditor::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FCustomDataAssetEditor::CanDuplicateNodes)
	);
}

void FCustomDataAssetEditor::SelectAllNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = CustomGraphWidget;
	if (CurrentGraphEditor.IsValid())
	{
		CurrentGraphEditor->SelectAllNodes();
	}
}

bool FCustomDataAssetEditor::CanSelectAllNodes()
{
	return true;
}

void FCustomDataAssetEditor::DeleteSelectedNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = CustomGraphWidget;
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
		UEdCustomGraphNode* EdNode = Cast<UEdCustomGraphNode>(*NodeIt);
		if (EdNode == nullptr || !EdNode->CanUserDeleteNode())
			continue;;

		if (UEdCustomGraphNode* EdNode_Node = Cast<UEdCustomGraphNode>(EdNode))
		{
			EdNode_Node->Modify();

			const UEdGraphSchema* Schema = EdNode_Node->GetSchema();
			if (Schema != nullptr)
			{
				Schema->BreakNodeLinks(*EdNode_Node);
			}

			EdNode_Node->DestroyNode();
		}
		else
		{
			EdNode->Modify();
			EdNode->DestroyNode();
		}
	}
}

bool FCustomDataAssetEditor::CanDeleteNodes()
{
	// If any of the nodes can be deleted then we should allow deleting
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdCustomGraphNode* Node = Cast<UEdCustomGraphNode>(*SelectedIter);
		if (Node != nullptr && Node->CanUserDeleteNode())
		{
			return true;
		}
	}

	return false;
}

void FCustomDataAssetEditor::DeleteSelectedDuplicatableNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = CustomGraphWidget;
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FGraphPanelSelectionSet OldSelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdCustomGraphNode* Node = Cast<UEdCustomGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}

	// Delete the duplicatable nodes
	DeleteSelectedNodes();

	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		if (UEdCustomGraphNode* Node = Cast<UEdCustomGraphNode>(*SelectedIter))
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}
}

void FCustomDataAssetEditor::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedDuplicatableNodes();
}

bool FCustomDataAssetEditor::CanCutNodes()
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FCustomDataAssetEditor::CopySelectedNodes()
{
	// Export the selected nodes and place the text on the clipboard
	FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	FString ExportedText;

	for (FGraphPanelSelectionSet::TIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdCustomGraphNode* Node = Cast<UEdCustomGraphNode>(*SelectedIter);
		if (Node == nullptr)
		{
			SelectedIter.RemoveCurrent();
			continue;
		}

		Node->PrepareForCopying();
	}

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool FCustomDataAssetEditor::CanCopyNodes()
{
	// If any of the nodes can be duplicated then we should allow copying
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdCustomGraphNode* Node = Cast<UEdCustomGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			return true;
		}
	}

	return false;
}

void FCustomDataAssetEditor::PasteNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = CustomGraphWidget;
	if (CurrentGraphEditor.IsValid())
	{
		PasteNodesHere(CurrentGraphEditor->GetPasteLocation());
	}
}

FGraphPanelSelectionSet FCustomDataAssetEditor::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	TSharedPtr<SGraphEditor> FocusedGraphEd = CustomGraphWidget;
	if (FocusedGraphEd.IsValid())
	{
		CurrentSelection = FocusedGraphEd->GetSelectedNodes();
	}

	return CurrentSelection;
}

void FCustomDataAssetEditor::PasteNodesHere(const FVector2D& Location)
{
	// Find the graph editor with focus
	TSharedPtr<SGraphEditor> CurrentGraphEditor = CustomGraphWidget;
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	// Select the newly pasted stuff
	UEdGraph* EdGraph = CurrentGraphEditor->GetCurrentGraph();
	{
		const FScopedTransaction Transaction(FGenericCommands::Get().Paste->GetDescription());
		EdGraph->Modify();

		// Clear the selection set (newly pasted stuff will be selected)
		CurrentGraphEditor->ClearSelectionSet();

		// Grab the text to paste from the clipboard.
		FString TextToImport;
		FPlatformApplicationMisc::ClipboardPaste(TextToImport);

		// Import the nodes
		TSet<UEdGraphNode*> PastedNodes;
		FEdGraphUtilities::ImportNodesFromText(EdGraph, TextToImport, PastedNodes);

		//Average position of nodes so we can move them while still maintaining relative distances to each other
		FVector2D AvgNodePosition(0.0f, 0.0f);

		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			UEdGraphNode* Node = *It;
			AvgNodePosition.X += Node->NodePosX;
			AvgNodePosition.Y += Node->NodePosY;
		}

		float InvNumNodes = 1.0f / float(PastedNodes.Num());
		AvgNodePosition.X *= InvNumNodes;
		AvgNodePosition.Y *= InvNumNodes;

		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			UEdGraphNode* Node = *It;
			CurrentGraphEditor->SetNodeSelection(Node, true);

			Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X;
			Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y;

			Node->SnapToGrid(16);

			// Give new node a different Guid from the old one
			Node->CreateNewGuid();
		}
	}

	// Update UI
	CurrentGraphEditor->NotifyGraphChanged();

	UObject* GraphOwner = EdGraph->GetOuter();
	if (GraphOwner)
	{
		GraphOwner->PostEditChange();
		GraphOwner->MarkPackageDirty();
	}
}

bool FCustomDataAssetEditor::CanPasteNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = CustomGraphWidget;
	if (!CurrentGraphEditor.IsValid())
	{
		return false;
	}

	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	return FEdGraphUtilities::CanImportNodesFromText(CurrentGraphEditor->GetCurrentGraph(), ClipboardContent);
}

void FCustomDataAssetEditor::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool FCustomDataAssetEditor::CanDuplicateNodes()
{
	return CanCopyNodes();
}

void FCustomDataAssetEditor::RebuildCustomData()
{
	if (CustomDataAsset == nullptr || CustomGraphWidget->GetCurrentGraph() == nullptr)
	{
		return;
	}

	CustomDataAsset->ClearGraph();
	
	UEdGraph* Graph = CustomGraphWidget->GetCurrentGraph();
	const TArray<TObjectPtr<class UEdGraphNode>>& Nodes = Graph->Nodes;

	for (int Index = 0; Index < Nodes.Num(); Index++)
	{
		if (UEdCustomGraphNode* EdNode = Cast<UEdCustomGraphNode>(Nodes[Index]))
		{
			UCustomGraphNodeAsset* GraphNodeAsset = EdNode->GraphNodeAsset;
			CustomDataAsset->AllNodes.Add(GraphNodeAsset);

			for (int PinIndex = 0; PinIndex < EdNode->Pins.Num(); ++PinIndex)
			{
				UEdGraphPin* Pin = EdNode->Pins[PinIndex];

				if (Pin->Direction != EEdGraphPinDirection::EGPD_Output)
				{
					continue;
				}

				for (int LinkToIndex = 0; LinkToIndex < Pin->LinkedTo.Num(); ++LinkToIndex)
				{
					UCustomGraphNodeAsset* ChildNode = nullptr;
					if(UEdCustomGraphNode* EdNode_Child = Cast<UEdCustomGraphNode>(Pin->LinkedTo[LinkToIndex]->GetOwningNode()))
					{
						ChildNode = EdNode_Child->GraphNodeAsset;
					}

					if (ChildNode != nullptr)
					{
						GraphNodeAsset->ChildrenNodes.Add(ChildNode);
						ChildNode->ParentNodes.Add(GraphNodeAsset);
					}
				}
			}
		}
	}

	for (int i = 0; i < CustomDataAsset->AllNodes.Num(); ++i)
	{
		UCustomGraphNodeAsset* Node = CustomDataAsset->AllNodes[i];
		if (Node->ParentNodes.IsEmpty())
		{
			CustomDataAsset->RootNodes.Add(Node);
		}
	}
}

#undef LOCTEXT_NAMESPACE