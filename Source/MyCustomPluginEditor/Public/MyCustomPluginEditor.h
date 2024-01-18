#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "CustomDataAssetActions.h"

/*
    Initializes and registers various components and actions related to custom asset management
*/

class FMyCustomPluginEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    TSharedPtr<FCustomDataAssetActions> CustomDataAssetActions;

    TSharedPtr<FGraphPanelNodeFactory> CustomPanelNodeFactory;
};