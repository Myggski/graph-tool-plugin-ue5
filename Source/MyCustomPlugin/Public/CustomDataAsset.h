#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CustomNodeData.h"
#include <CustomGraphNodeAsset.h>
#include "CustomDataAsset.generated.h"

UCLASS(BlueprintType)
class MYCUSTOMPLUGIN_API UCustomDataAsset : public UObject
{
    GENERATED_BODY()

public:
    UCustomDataAsset() = default;

public:
    UFUNCTION(BlueprintCallable, Category = "Custom Graph")
    const TArray<UCustomGraphNodeAsset*>& GetRootNodes() const;

    UFUNCTION(BlueprintCallable, Category = "Custom Graph")
    int GetLevelNum() const;

    UFUNCTION(BlueprintCallable, Category = "Custom Graph")
    const TArray<UCustomGraphNodeAsset*> GetNodesByLevel(int Level);

    UFUNCTION(BlueprintCallable, Category = "Custom Graph")
    void ClearGraph();

public:
    UPROPERTY(BlueprintReadOnly)
    TArray<FCustomNodeData> NodeTypeList;

    UPROPERTY(BlueprintReadOnly, Category = "Custom Graph")
    TArray<UCustomGraphNodeAsset*> RootNodes;

    UPROPERTY(BlueprintReadOnly, Category = "Custom Graph")
    TArray<UCustomGraphNodeAsset*> AllNodes;

#if WITH_EDITORONLY_DATA
    UPROPERTY()
    class UEdGraph* EdGraph;
#endif
};