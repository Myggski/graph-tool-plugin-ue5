#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class UEdCustomGraphNode;

/*
    Responsible for how the custom graph node will look like in the graph, but also how to interact with it
*/

class MYCUSTOMPLUGINEDITOR_API SCustomGraphNode : public SGraphNode
{
public:
    SLATE_BEGIN_ARGS(SCustomGraphNode) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, UEdCustomGraphNode* InNode);
    virtual void UpdateGraphNode() override;
    virtual void CreatePinWidgets() override;
    virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;

    FSlateColor GetBorderBackgroundColor() const;
    const FSlateBrush* GetNameIcon() const;
    UEdCustomGraphNode* Node;
};