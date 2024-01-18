#include "Kismet2/Kismet2NameValidators.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "AssetEditor/EdCustomGraphNode.h"

#define LOCTEXT_NAMESPACE "EdNode_CustomGraphNode"

UEdCustomGraphNode::UEdCustomGraphNode()
{
	bCanRenameNode = true;
}

UEdCustomGraphNode::~UEdCustomGraphNode()
{

}

void UEdCustomGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, "MultipleNodes", FName(), TEXT("In"));
	CreatePin(EGPD_Output, "MultipleNodes", FName(), TEXT("Out"));
}

FText UEdCustomGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(GraphNodeAsset->Name);
}

FText UEdCustomGraphNode::GetTooltipText() const
{
	return FText::FromString(GraphNodeAsset->Name);
}

FLinearColor UEdCustomGraphNode::GetBackgroundColor() const
{
	return FLinearColor::Black;
}

FLinearColor UEdCustomGraphNode::GetBorderColor() const
{
	return FLinearColor(.83f, .83f, .83f);
}

UEdGraphPin* UEdCustomGraphNode::GetInputPin() const
{
	return Pins[0];
}

UEdGraphPin* UEdCustomGraphNode::GetOutputPin() const
{
	return Pins[1];
}

void UEdCustomGraphNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);

	if (FromPin != nullptr)
	{
		if (GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
	}
}


#undef LOCTEXT_NAMESPACE