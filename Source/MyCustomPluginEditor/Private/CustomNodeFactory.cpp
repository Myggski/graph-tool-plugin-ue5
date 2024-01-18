#include "CustomNodeFactory.h"
#include "EdGraph/EdGraphNode.h"
#include "AssetEditor/EdCustomGraphNode.h"
#include "AssetEditor/SCustomGraphNode.h"

TSharedPtr<class SGraphNode> FCustomPanelNodeFactory::CreateNode(UEdGraphNode* Node) const
{
	if (UEdCustomGraphNode* EdNode = Cast<UEdCustomGraphNode>(Node))
	{
		return SNew(SCustomGraphNode, EdNode);
	}

	return nullptr;
}