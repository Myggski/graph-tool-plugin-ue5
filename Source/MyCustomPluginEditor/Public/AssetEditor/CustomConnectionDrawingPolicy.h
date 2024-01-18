#pragma once

#include "CoreMinimal.h"
#include "ConnectionDrawingPolicy.h"

/*
	Style policy for the edges between nodes (I added this for hover-effect on an edge)
*/

class MYCUSTOMPLUGINEDITOR_API FCustomConnectionDrawingPolicy : public FConnectionDrawingPolicy
{
public:
	FCustomConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj);

	virtual void DrawConnection(int32 LayerId, const FVector2D& Start, const FVector2D& End, const FConnectionParams& Params) override;

	virtual void DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, /*inout*/ FConnectionParams& Params) override;

protected:
	UEdGraph* GraphObj;
	TMap<UEdGraphNode*, int32> NodeWidgetMap;
};