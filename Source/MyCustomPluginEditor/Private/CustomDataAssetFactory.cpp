#include "CustomDataAssetFactory.h"
#include "CustomDataAsset.h"

UCustomDataAssetFactory::UCustomDataAssetFactory()
{
    // Assign the class that this factory will create
    SupportedClass = UCustomDataAsset::StaticClass();
    bCreateNew = true;
    bEditAfterNew = true;
}

UObject* UCustomDataAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    UCustomDataAsset* NewAsset = NewObject<UCustomDataAsset>(InParent, InClass, InName, Flags);
    return NewAsset;
}