#include "CustomDataAssetActions.h"
#include "CustomDataAsset.h"
#include "CustomDataAssetEditor.h"

FText FCustomDataAssetActions::GetName() const
{
    return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_CustomDataAsset", "Custom Data Asset");
}

FColor FCustomDataAssetActions::GetTypeColor() const
{
    return FColor::Green;
}

UClass* FCustomDataAssetActions::GetSupportedClass() const
{
    return UCustomDataAsset::StaticClass();
}

uint32 FCustomDataAssetActions::GetCategories()
{
    return AssetCategory;
}

void FCustomDataAssetActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
    const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

    for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
    {
        if (UCustomDataAsset* DataAsset = Cast<UCustomDataAsset>(*ObjIt))
        {
            TSharedRef<FCustomDataAssetEditor> NewGraphEditor(new FCustomDataAssetEditor());
            NewGraphEditor->InitCustomDataAssetEditor(Mode, EditWithinLevelEditor, DataAsset);
        }
    }
}