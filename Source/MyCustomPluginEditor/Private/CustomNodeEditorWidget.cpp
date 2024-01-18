#include "CustomNodeEditorWidget.h"
#include "AssetEditor/EdCustomGraphNode.h"
#include "AssetEditor/CustomDragConnection.h"

void SCustomNodeEditorWidget::Construct(const FArguments& InArgs)
{
    CustomDataAsset = InArgs._CustomDataAsset;
    CustomGraphWidget = InArgs._CustomGraphWidget;
    PropertyWidget = InArgs._PropertyWidget;

    if (CustomDataAsset)
    {
        const TArray<FCustomNodeData>& AssetNodes = CustomDataAsset->NodeTypeList;
        Nodes.Empty();

        for (const FCustomNodeData& Node : AssetNodes)
        {
            Nodes.Add(MakeShared<FCustomNodeData>(Node)); // Allocate new instances using MakeShared
        }
    }

    ChildSlot
        [
            SNew(SVerticalBox)

            + SVerticalBox::Slot()
                [
                    SAssignNew(ListViewWidget, SListView<TSharedPtr<FCustomNodeData>>)
                    .ListItemsSource(&Nodes)
                    .OnGenerateRow(this, &SCustomNodeEditorWidget::GenerateRowWidget)
                    .OnSelectionChanged(this, &SCustomNodeEditorWidget::OnSelectedNodesChanged)
                ]

            + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
                .FillWidth(1.f)
                [
                    SAssignNew(NodeNameTextBox, SEditableTextBox)
                        .HintText(FText::FromString("Enter Node Name"))
                        .OnTextCommitted(this, &SCustomNodeEditorWidget::HandleNodeNameCommitted)
                ]

            + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SButton)
                        .Text(FText::FromString("Add"))
                        .OnClicked(this, &SCustomNodeEditorWidget::HandleAddButtonClicked)
                        .IsEnabled(this, &SCustomNodeEditorWidget::IsAddButtonEnabled)
                ]
        ]
    ];
}

void SCustomNodeEditorWidget::OnSelectedNodesChanged(TSharedPtr<FCustomNodeData> SelectedItem, ESelectInfo::Type SelectInfo)
{
    if (!PropertyWidget)
    {
        return;
    }

    // Check if a node is selected
    if (SelectedItem.IsValid())
    {
        // Handle node selection here
        // For example, you can update the PropertyWidget with the selected node's properties
        PropertyWidget->SetObject(CustomDataAsset);
    }
}

void SCustomNodeEditorWidget::UpdateNodes(const TArray<FCustomNodeData>& NewNodes)
{
    // Convert the array of custom data to an array of shared pointers
    TArray<TSharedPtr<FCustomNodeData>> SharedPtrNodes;
    for (const FCustomNodeData& Node : NewNodes)
    {
        SharedPtrNodes.Add(MakeShared<FCustomNodeData>(Node));
    }

    // Clear the array and append the new nodes
    Nodes.Empty();
    Nodes.Append(SharedPtrNodes);

    if (ListViewWidget.IsValid())
    {
        ListViewWidget->RequestListRefresh();
    }
}

FReply SCustomNodeEditorWidget::HandleAddButtonClicked()
{
    if (NodeNameTextBox.IsValid())
    {
        // Create a new node with the entered name
        FCustomNodeData NewNode;
        NewNode.Name = NodeNameTextBox->GetText().ToString();
        Nodes.Add(MakeShared<FCustomNodeData>(NewNode));

        if (ListViewWidget.IsValid())
        {
            ListViewWidget->RequestListRefresh();
        }

        // Clear the node name text box
        NodeNameTextBox->SetText(FText::GetEmpty());

        SaveCustomDataAsset();
    }

    return FReply::Handled();
}

void SCustomNodeEditorWidget::HandleNodeNameCommitted(const FText& NewText, ETextCommit::Type CommitType)
{
    if (CommitType == ETextCommit::OnEnter)
    {
        // Create a new node with the entered name
        FCustomNodeData NewNode;
        NewNode.Name = NewText.ToString();
        Nodes.Add(MakeShared<FCustomNodeData>(NewNode));

        if (ListViewWidget.IsValid())
        {
            ListViewWidget->RequestListRefresh();
        }

        // Clear the node name text box
        NodeNameTextBox->SetText(FText::GetEmpty());

        SaveCustomDataAsset();
    }
}

void SCustomNodeEditorWidget::SaveCustomDataAsset()
{
    if (CustomDataAsset)
    {
        // Update the nodes in the CustomDataAsset with the added/removed nodes
        TArray<FCustomNodeData> AssetNodes;
        for (const TSharedPtr<FCustomNodeData>& Node : Nodes)
        {
            AssetNodes.Add(*Node);
        }

        CustomDataAsset->NodeTypeList = MoveTemp(AssetNodes);
    }
}

TSharedRef<ITableRow> SCustomNodeEditorWidget::GenerateRowWidget(TSharedPtr<FCustomNodeData> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
    return SNew(STableRow<TSharedPtr<FCustomNodeData>>, OwnerTable)
        .Padding(FMargin(8.f, 8.f, 8.f, 8.f))
        .OnDragDetected_Lambda([this, Item](const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)->FReply
        {
            return HandleDragDetected(MyGeometry, MouseEvent, Item);
        })
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                SNew(STextBlock)
                .Text(FText::FromString(Item->Name))
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            .HAlign(HAlign_Right)
            [
                SNew(SButton)
                    .OnClicked_Lambda([this, Item]() -> FReply
                    {
                        Nodes.Remove(Item);
                        SaveCustomDataAsset();
                        ListViewWidget->RequestListRefresh();
                        return FReply::Handled();
                    })
                    .ButtonStyle(FCoreStyle::Get(), "NoBorder")
                    .ForegroundColor(FSlateColor(FLinearColor::Red))
                    .ToolTipText(FText::FromString("Delete"))
                    [
                        SNew(STextBlock)
                            .Text(FText::FromString("X"))
                    ]
            ]
        ];
}

FReply SCustomNodeEditorWidget::HandleDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, TSharedPtr<FCustomNodeData> Item)
{
    if (Item.IsValid())
    {
        SGraphPanel* GraphPanelPtr = CustomGraphWidget->GetGraphPanel();
        
        DragDropOperation = FCustomDragConnection::New(GraphPanelPtr, Item); // Create an instance using MakeShareable

        DraggedItem = Item;

        return FReply::Handled().BeginDragDrop(DragDropOperation.ToSharedRef());
    }

    return FReply::Unhandled();
}

bool SCustomNodeEditorWidget::IsAddButtonEnabled() const
{
    return NodeNameTextBox.IsValid() && !NodeNameTextBox->GetText().IsEmptyOrWhitespace();
}