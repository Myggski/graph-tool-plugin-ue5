#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "EdCustomGraph.generated.h"

class UCustomDataAsset;

/*
	It's responsible for the properties of the custom graph, behavior and serialization data. Right now it doesn't do much
*/

UCLASS()
class MYCUSTOMPLUGINEDITOR_API UEdCustomGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	UEdCustomGraph() = default;
	virtual ~UEdCustomGraph() = default;

	UCustomDataAsset* GetCustomData() const;

	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	virtual void PostEditUndo() override;
};