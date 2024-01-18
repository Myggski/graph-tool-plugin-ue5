#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Factories/Factory.h"
#include "CustomDataAssetFactory.generated.h"

/*
    Creates the custom data type that will hold all the information that the tool needs.
*/

UCLASS()
class MYCUSTOMPLUGINEDITOR_API UCustomDataAssetFactory : public UFactory
{
    GENERATED_BODY()

public:
    UCustomDataAssetFactory();

    // UFactory interface
    virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
        UObject* Context, FFeedbackContext* Warn) override;
    // End of UFactory interface
};