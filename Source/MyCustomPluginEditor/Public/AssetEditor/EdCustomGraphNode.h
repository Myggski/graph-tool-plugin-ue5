#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "CustomDataAsset.h"
#include "SCustomGraphNode.h"
#include "EdCustomGraphNode.generated.h"

/*
	It's responsible for the properties of the custom node, behavior and serialization data. 
*/

UCLASS(MinimalAPI)
class UEdCustomGraphNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UEdCustomGraphNode();
	virtual ~UEdCustomGraphNode();

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetBackgroundColor() const;
	virtual FLinearColor GetBorderColor() const;
	virtual FText GetTooltipText() const override;
	virtual UEdGraphPin* GetInputPin() const;
	virtual UEdGraphPin* GetOutputPin() const;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;

public:
	UPROPERTY(VisibleAnywhere, Instanced, Category = "Custom Node")
	UCustomGraphNodeAsset* GraphNodeAsset;

	SCustomGraphNode* SEdNode;
};