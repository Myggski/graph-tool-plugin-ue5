#include "AssetEditor/CustomDragConnection.h"
#include "AssetEditor/EdCustomGraphNode.h"
#include "SGraphPanel.h"

TSharedRef<FCustomDragConnection> FCustomDragConnection::New(SGraphPanel* GraphPanel, TSharedPtr<FCustomNodeData> InCustomNodeData)
{
	TSharedRef<FCustomDragConnection> Operation = MakeShareable(new FCustomDragConnection(GraphPanel, InCustomNodeData.Get()));
	Operation->Construct();

	return Operation;
}

FReply FCustomDragConnection::DroppedOnPanel(const TSharedRef<SWidget>& Panel, FVector2D ScreenPosition, FVector2D GraphPosition, UEdGraph& Graph)
{
	GraphPanel->OnStopMakingConnection();

	// Get the geometry of the graph panel
	const FGeometry& PanelGeometry = GraphPanel->GetCachedGeometry();

	// Convert the screen position to local coordinates relative to the graph panel
	FVector2D LocalPosition = PanelGeometry.AbsoluteToLocal(ScreenPosition);

	// Convert the local position to panel coordinates within the graph panel
	FVector2D PanelPosition = LocalPosition / PanelGeometry.Scale;

	// Convert the panel coordinates to graph coordinates
	FVector2D GraphCoord = GraphPanel->PanelCoordToGraphCoord(PanelPosition);

	// Create a new node in the graph at the local position
	FGraphNodeCreator<UEdCustomGraphNode> NodeCreator(Graph);
	UEdCustomGraphNode* NewNode = NodeCreator.CreateNode();

	UCustomGraphNodeAsset* NewNodeAsset = NewObject<UCustomGraphNodeAsset>(NewNode);
	NewNodeAsset->Name = CustomNodeData->Name;

	NewNode->SetFlags(RF_Transactional);
	NewNode->GraphNodeAsset = NewNodeAsset;
	NewNode->bCanRenameNode = true;
	NewNode->NodePosX = GraphCoord.X;
	NewNode->NodePosY = GraphCoord.Y;
	
	NodeCreator.Finalize();

	return FReply::Handled().EndDragDrop();
}

FCustomDragConnection::FCustomDragConnection(SGraphPanel* GraphPanelIn, FCustomNodeData* CustomNodeDataIn)
	: GraphPanel(GraphPanelIn), 
	CustomNodeData(CustomNodeDataIn),
	DecoratorAdjust(FSlateApplication::Get().GetCursorSize()) { }