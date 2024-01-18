#pragma once

#include "EdGraphUtilities.h"
#include "EdGraph/EdGraphNode.h"

/*
	Creates custom node
*/

class FCustomPanelNodeFactory : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override;
};