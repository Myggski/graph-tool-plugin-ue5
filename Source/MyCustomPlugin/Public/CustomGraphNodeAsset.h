#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CustomGraphNodeAsset.generated.h"

UCLASS(BlueprintType, EditInlineNew)
class MYCUSTOMPLUGIN_API UCustomGraphNodeAsset : public UObject
{
    GENERATED_BODY()

public:
    UCustomGraphNodeAsset();

    UFUNCTION(BlueprintCallable, Category = "GenericGraphNode")
    bool IsLeafNode() const;
public:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString Name;

    UPROPERTY(BlueprintReadOnly, Category = "Custom Node Asset")
    TArray<UCustomGraphNodeAsset*> ParentNodes;

    UPROPERTY(BlueprintReadOnly, Category = "Custom Node Asset")
    TArray<UCustomGraphNodeAsset*> ChildrenNodes;
};