#pragma once 

#include "Widgets/Views/SListView.h"
#include "SlateBasics.h"
#include "CustomNodeData.h"
#include "CustomDataAsset.h"

/*
    This widget displays, creates and removes "unique" node items in a list.
    You can drag and drop an item into a graph and it will generate a node in the graph
*/

class SCustomNodeEditorWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SCustomNodeEditorWidget) 
        : _CustomDataAsset(nullptr),
        _CustomGraphWidget(nullptr),
        _PropertyWidget(nullptr) {}
    SLATE_ARGUMENT(UCustomDataAsset*, CustomDataAsset)
    SLATE_ARGUMENT(SGraphEditor*, CustomGraphWidget)
    SLATE_ARGUMENT(IDetailsView*, PropertyWidget)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    void UpdateNodes(const TArray<FCustomNodeData>& NewNodes);

private:
    FReply HandleAddButtonClicked();
    FReply HandleDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, TSharedPtr<FCustomNodeData> Item);
    void HandleNodeNameCommitted(const FText& NewText, ETextCommit::Type CommitType);
    void SaveCustomDataAsset();
    void OnSelectedNodesChanged(TSharedPtr<FCustomNodeData> SelectedItem, ESelectInfo::Type SelectInfo);
    bool IsAddButtonEnabled() const;
    TSharedRef<ITableRow> GenerateRowWidget(TSharedPtr<FCustomNodeData> Item, const TSharedRef<STableViewBase>& OwnerTable);

private:
    TArray<TSharedPtr<FCustomNodeData>> Nodes;

    TSharedPtr<SListView<TSharedPtr<FCustomNodeData>>> ListViewWidget;
    TSharedPtr<FCustomNodeData> DraggedItem;
    TSharedPtr<SEditableTextBox> NodeNameTextBox;
    TSharedPtr<SButton> AddButton;

    class IDetailsView* PropertyWidget;
    SGraphEditor* CustomGraphWidget;
    UCustomDataAsset* CustomDataAsset;

    TSharedPtr<FCustomDragConnection> DragDropOperation; // Declare DragDropOperation as a pointer
};