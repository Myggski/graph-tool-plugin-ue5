#include "MyCustomPluginEditor.h"
#include "AssetToolsModule.h"
#include "CustomDataAsset.h"
#include "CustomDataAssetActions.h"
#include <CustomNodeFactory.h>

#define LOCTEXT_NAMESPACE "FMyCustomPluginEditorModule"

// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
void FMyCustomPluginEditorModule::StartupModule()
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

    // Register the asset category
    const EAssetTypeCategories::Type MyCustomAssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("MyCustomAsset")), LOCTEXT("ChiefRebelCategory", "Custom Graph Tool"));

    // This code registers the custom panel node factory with the graph editor, allowing the factory to create custom visual nodes within the editor.
    CustomPanelNodeFactory = MakeShareable(new FCustomPanelNodeFactory());
    FEdGraphUtilities::RegisterVisualNodeFactory(CustomPanelNodeFactory);

    // Register the asset type actions
    CustomDataAssetActions = MakeShareable(new FCustomDataAssetActions(MyCustomAssetCategory));
    AssetTools.RegisterAssetTypeActions(CustomDataAssetActions.ToSharedRef());
}

void FMyCustomPluginEditorModule::ShutdownModule()
{
    if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
    {
        IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();

        // Unregister asset type actions associated with your module's class name
        TArray<UClass*> ClassesToUnregister;
        ClassesToUnregister.Add(UCustomDataAsset::StaticClass());  // Replace UCustomDataAsset with your custom asset class

        for (UClass* Class : ClassesToUnregister)
        {
            TWeakPtr<IAssetTypeActions> AssetTypeActions = AssetTools.GetAssetTypeActionsForClass(Class);
            if (AssetTypeActions.IsValid())
            {
                AssetTools.UnregisterAssetTypeActions(AssetTypeActions.Pin().ToSharedRef());
            }
        }
    }

    if (CustomPanelNodeFactory.IsValid())
    {
        FEdGraphUtilities::UnregisterVisualNodeFactory(CustomPanelNodeFactory);
        CustomPanelNodeFactory.Reset();
    }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMyCustomPluginEditorModule, MyCustomPluginEditor)