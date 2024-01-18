#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "CustomDataAsset.h"

/*
    This class explains how the content browser is going to interact with the custom asset type
    Which category it's going to show up in, what color the icon should have and such
*/

class MYCUSTOMPLUGINEDITOR_API FCustomDataAssetActions : public FAssetTypeActions_Base
{
public:
    // Constructor
    FCustomDataAssetActions(const EAssetTypeCategories::Type InAssetCategory)
        : AssetCategory(InAssetCategory) { }

    // FAssetTypeActions_Base overrides
    virtual FText GetName() const override;
    virtual FColor GetTypeColor() const override;
    virtual UClass* GetSupportedClass() const override;
    virtual uint32 GetCategories() override;
    virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor) override;

private:
    EAssetTypeCategories::Type AssetCategory;
};