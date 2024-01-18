#include "AssetEditor/EdCustomGraphSchema.h"
#include "AssetEditor/EdCustomGraphNode.h"
#include "AssetEditor/CustomConnectionDrawingPolicy.h"

#define LOCTEXT_NAMESPACE "CustomGraphSchema"

class FNodeVisitorCycleChecker
{
public:
	/** Check whether a loop in the graph would be caused by linking the passed-in nodes */
	bool CheckForLoop(UEdGraphNode* StartNode, UEdGraphNode* EndNode)
	{

		VisitedNodes.Add(StartNode);

		return TraverseNodes(EndNode);
	}

private:
	bool TraverseNodes(UEdGraphNode* Node)
	{
		VisitedNodes.Add(Node);

		for (auto MyPin : Node->Pins)
		{
			if (MyPin->Direction == EGPD_Output)
			{
				for (auto OtherPin : MyPin->LinkedTo)
				{
					UEdGraphNode* OtherNode = OtherPin->GetOwningNode();
					if (VisitedNodes.Contains(OtherNode))
					{
						// Only  an issue if this is a back-edge
						return false;
					}
					else if (!FinishedNodes.Contains(OtherNode))
					{
						// Only should traverse if this node hasn't been traversed
						if (!TraverseNodes(OtherNode))
							return false;
					}
				}
			}
		}

		VisitedNodes.Remove(Node);
		FinishedNodes.Add(Node);
		return true;
	};


	TSet<UEdGraphNode*> VisitedNodes;
	TSet<UEdGraphNode*> FinishedNodes;
};

bool UEdCustomGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	// We don't actually care about the pin, we want the node that is being dragged between
	UEdCustomGraphNode* NodeA = Cast<UEdCustomGraphNode>(A->GetOwningNode());
	UEdCustomGraphNode* NodeB = Cast<UEdCustomGraphNode>(B->GetOwningNode());

	// Check that this edge doesn't already exist
	for (UEdGraphPin* TestPin : NodeA->GetOutputPin()->LinkedTo)
	{
		UEdGraphNode* ChildNode = TestPin->GetOwningNode();
		if (UEdCustomGraphNode* EdNode_Edge = Cast<UEdCustomGraphNode>(ChildNode))
		{
			if (!EdNode_Edge->GetOutputPin()->LinkedTo.IsEmpty())
			{
				ChildNode = Cast<UEdCustomGraphNode>(EdNode_Edge->GetOutputPin()->LinkedTo[0]->GetOwningNode());
			}
		}

		if (ChildNode == NodeB)
			return false;
	}

	if (NodeA && NodeB)
	{
		Super::TryCreateConnection(NodeA->GetOutputPin(), NodeB->GetInputPin());
		return true;
	}

	return false;
}

const FPinConnectionResponse UEdCustomGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	// Make sure the pins are not on the same node
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorSameNode", "Can't connect node to itself"));
	}

	const UEdGraphPin* Out = A;
	const UEdGraphPin* In = B;

	UEdCustomGraphNode* EdNode_Out = Cast<UEdCustomGraphNode>(Out->GetOwningNode());
	UEdCustomGraphNode* EdNode_In = Cast<UEdCustomGraphNode>(In->GetOwningNode());

	if (EdNode_Out == nullptr || EdNode_In == nullptr)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinError", "Not a valid UGenericGraphEdNode"));
	}

	//Determine if we can have cycles or not
	bool bAllowCycles = false;

	// check for cycles
	FNodeVisitorCycleChecker CycleChecker;
	if (!bAllowCycles && !CycleChecker.CheckForLoop(Out->GetOwningNode(), In->GetOwningNode()))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorCycle", "Can't create a graph cycle"));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE, LOCTEXT("PinConnect", "Connect nodes with edge"));
}

void UEdCustomGraphSchema::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakNodeLinks", "Break Node Links"));

	UEdGraphSchema::BreakNodeLinks(TargetNode);
}

void UEdCustomGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakPinLinks", "Break Pin Links"));

	UEdGraphSchema::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void UEdCustomGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakSinglePinLink", "Break Pin Link"));

	UEdGraphSchema::BreakSinglePinLink(SourcePin, TargetPin);
}

bool UEdCustomGraphSchema::SupportsDropPinOnNode(UEdGraphNode* InTargetNode, const FEdGraphPinType& InSourcePinType, EEdGraphPinDirection InSourcePinDirection, FText& OutErrorMessage) const
{
	return Cast<UEdCustomGraphNode>(InTargetNode) != nullptr;
}

UEdGraphPin* UEdCustomGraphSchema::DropPinOnNode(UEdGraphNode* InTargetNode, const FName& InSourcePinName, const FEdGraphPinType& InSourcePinType, EEdGraphPinDirection InSourcePinDirection) const
{
	UEdCustomGraphNode* EdNode = Cast<UEdCustomGraphNode>(InTargetNode);
	switch (InSourcePinDirection)
	{
	case EGPD_Input:
		return EdNode->GetOutputPin();
	case EGPD_Output:
		return EdNode->GetInputPin();
	default:
		return nullptr;
	}
}

bool UEdCustomGraphSchema::CreateAutomaticConversionNodeAndConnections(UEdGraphPin* A, UEdGraphPin* B) const
{
	UEdCustomGraphNode* NodeA = Cast<UEdCustomGraphNode>(A->GetOwningNode());
	UEdCustomGraphNode* NodeB = Cast<UEdCustomGraphNode>(B->GetOwningNode());

	// Are nodes and pins all valid?
	if (!NodeA || !NodeA->GetOutputPin() || !NodeB || !NodeB->GetInputPin())
		return false;

	const auto& PinA = NodeA->GetOutputPin();
	const auto& PinB = NodeB->GetInputPin();

	// Create a connection from PinA to PinB
	PinA->Modify();
	PinB->Modify();
	PinA->MakeLinkTo(PinB);

	// Notify the nodes about the connection change
	NodeA->PinConnectionListChanged(PinA);
	NodeB->PinConnectionListChanged(PinB);

	return true;
}

class FConnectionDrawingPolicy* UEdCustomGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	return new FCustomConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

#undef LOCTEXT_NAMESPACE
