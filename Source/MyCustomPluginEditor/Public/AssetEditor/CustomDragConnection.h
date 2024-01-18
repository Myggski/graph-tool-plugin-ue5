#pragma once

#include "CoreMinimal.h"
#include "Input/DragAndDrop.h"
#include "Input/Reply.h"
#include "Widgets/SWidget.h"
#include "GraphEditorDragDropAction.h"

class SGraphPanel;

/*
	Responsible of drag / drop events for list items into a graph (creating nodes in a graph when dropped)
*/

class FCustomDragConnection : public FGraphEditorDragDropAction
{
public:
	DRAG_DROP_OPERATOR_TYPE(FDragConnection, FGraphEditorDragDropAction)

	static TSharedRef<FCustomDragConnection> New(SGraphPanel* GraphPanel, TSharedPtr<FCustomNodeData> InCustomNodeData);

	virtual FReply DroppedOnPanel(const TSharedRef<SWidget>& Panel, FVector2D ScreenPosition, FVector2D GraphPosition, UEdGraph& Graph) override;

protected:
	typedef FGraphEditorDragDropAction Super;

	FCustomDragConnection(SGraphPanel* GraphPanelIn, FCustomNodeData* CustomNodeDataIn);

protected:
	SGraphPanel* GraphPanel;
	FCustomNodeData* CustomNodeData;

	FVector2D DecoratorAdjust;
};
