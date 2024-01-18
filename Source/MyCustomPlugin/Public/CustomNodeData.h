#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CustomNodeData.generated.h"

/*
    This class is used to list the unique nodes in a list viewer
    May change this to a UObject in order to change it's name in the Property-widget
*/

USTRUCT(Blueprintable)
struct FCustomNodeData
{
    GENERATED_USTRUCT_BODY()

public:
    FCustomNodeData() = default;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Node")
    FString Name;
};