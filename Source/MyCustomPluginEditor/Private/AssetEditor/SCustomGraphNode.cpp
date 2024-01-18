#include "AssetEditor/SCustomGraphNode.h"
#include "AssetEditor/EdCustomGraphNode.h"
#include "SGraphNode.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "SGraphPin.h"
#include "AssetEditor/CustomPinDragDropConnection.h"

#define LOCTEXT_NAMESPACE "EdNode_GenericGraph"

class SGenericGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SGenericGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		this->SetCursor(EMouseCursor::Default);

		bShowLabel = true;

		GraphPinObj = InPin;
		check(GraphPinObj != nullptr);

		const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
		check(Schema);

		SBorder::Construct(SBorder::FArguments()
			.BorderImage(this, &SGenericGraphPin::GetPinBorder)
			.BorderBackgroundColor(this, &SGenericGraphPin::GetPinColor)
			.OnMouseButtonDown(this, &SGenericGraphPin::OnPinMouseDown)
			.Cursor(this, &SGenericGraphPin::GetPinCursor)
			.Padding(FMargin(5.0f))
		);
	}

protected:
	virtual FSlateColor GetPinColor() const override
	{
		return FLinearColor::White;
	}

	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override
	{
		return SNew(STextBlock);
	}

	const FSlateBrush* GetPinBorder() const
	{
		return FAppStyle::GetBrush(TEXT("Graph.StateNode.Body"));
	}


	virtual TSharedRef<FDragDropOperation> SpawnPinDragEvent(const TSharedRef<class SGraphPanel>& InGraphPanel, const TArray< TSharedRef<SGraphPin> >& InStartingPins) override
	{
		FCustomPinDragDropConnection::FDraggedPinTable PinHandles;
		PinHandles.Reserve(InStartingPins.Num());

		for (const TSharedRef<SGraphPin>& PinWidget : InStartingPins)
		{
			PinHandles.Add(PinWidget->GetPinObj());
		}

		return FCustomPinDragDropConnection::New(InGraphPanel, PinHandles);
	}
};


void SCustomGraphNode::Construct(const FArguments& InArgs, UEdCustomGraphNode* InNode)
{
	GraphNode = InNode;
	Node = InNode;
	UpdateGraphNode();
	InNode->SEdNode = this;
}

void SCustomGraphNode::UpdateGraphNode()
{
	TSharedPtr<SVerticalBox> NodeBody;
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, Node);

	auto Derp = NodeTitle->GetHeadTitle();

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);

	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0.0f)
			.BorderBackgroundColor(this, &SCustomGraphNode::GetBorderBackgroundColor)
			[
				SNew(SOverlay)

				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SHorizontalBox)

					// Input Pin Area
					+ SHorizontalBox::Slot()
					.FillWidth(1)
					[
						SAssignNew(LeftNodeBox, SVerticalBox)
					]

					// Output Pin Area	
					+ SHorizontalBox::Slot()
					.FillWidth(1)
					[
						SAssignNew(RightNodeBox, SVerticalBox)
					]
				]

				+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(FMargin(1.f))
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("Graph.StateNode.ColorSpill"))
					.BorderBackgroundColor(FLinearColor(0.6f, 0.6f, 0.6f))
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Visibility(EVisibility::SelfHitTestInvisible)
					.Padding(FMargin(8.f))
					[
						SAssignNew(NodeBody, SVerticalBox)

						// Title
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SHorizontalBox)

						// Node Title
						+ SHorizontalBox::Slot()
						.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.f))
						[
							SNew(SVerticalBox)
							+ SVerticalBox::Slot()
							.AutoHeight()
							[
								SAssignNew(InlineEditableText, SInlineEditableTextBlock)
								.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
								.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
								.IsSelected(this, &SCustomGraphNode::IsSelectedExclusively)
								.IsReadOnly(true)
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

	CreatePinWidgets();
}

void SCustomGraphNode::CreatePinWidgets()
{
	UEdCustomGraphNode* StateNode = CastChecked<UEdCustomGraphNode>(GraphNode);

	for (int32 PinIdx = 0; PinIdx < StateNode->Pins.Num(); PinIdx++)
	{
		UEdGraphPin* MyPin = StateNode->Pins[PinIdx];
		if (!MyPin->bHidden)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SGenericGraphPin, MyPin);

			AddPin(NewPin.ToSharedRef());
		}
	}
}

void SCustomGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = PinObj && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility( TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced) );
	}

	TSharedPtr<SVerticalBox> PinBox;
	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		PinBox = LeftNodeBox;
		InputPins.Add(PinToAdd);
	}
	else // Direction == EEdGraphPinDirection::EGPD_Output
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

FSlateColor SCustomGraphNode::GetBorderBackgroundColor() const
{
	UEdCustomGraphNode* MyNode = CastChecked<UEdCustomGraphNode>(GraphNode);
	return MyNode ? MyNode->GetBorderColor() : FLinearColor(1.f, 1.f, 0.0f);
}

const FSlateBrush* SCustomGraphNode::GetNameIcon() const
{
	return FAppStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Icon"));
}


#undef LOCTEXT_NAMESPACE