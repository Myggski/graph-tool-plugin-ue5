#pragma once

#include "CustomNodeEditorWidget.h"

class UCustomDataAsset;

/*
     This class is responsible for arranging the layout and adding widgets to the tool window.
     It also is resposnible for the commands for the graph widget (should maybe be moved to another place?)
*/

class FCustomDataAssetEditor : public FAssetEditorToolkit
{
public:
    void InitCustomDataAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UCustomDataAsset* InCustomDataAsset);

    virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
    virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;

    // FAssetEditorToolkit overrides
    virtual FName GetToolkitFName() const override;
    virtual FText GetBaseToolkitName() const override;
    virtual FText GetToolkitName() const override;
    virtual FLinearColor GetWorldCentricTabColorScale() const override;
    virtual FString GetWorldCentricTabPrefix() const override;
    virtual void SaveAsset_Execute() override;

    TSharedRef<SDockTab> SpawnTab_RuleList(const FSpawnTabArgs& Args);
    TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);
    TSharedRef<SDockTab> SpawnTab_NodeList(const FSpawnTabArgs& Args);
    // ... additional necessary overrides for FAssetEditorToolkit

protected:
    void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection);
    TSharedRef<SGraphEditor> CreateGraphtWidget();
    void CreateInternalWidgets();
    void CreateEdGraph();
    void CreateCommandList();

        // Delegates for graph editor commands
    void SelectAllNodes();
    bool CanSelectAllNodes();
    void DeleteSelectedNodes();
    bool CanDeleteNodes();
    void DeleteSelectedDuplicatableNodes();
    void CutSelectedNodes();
    bool CanCutNodes();
    void CopySelectedNodes();
    bool CanCopyNodes();
    void PasteNodes();
    void PasteNodesHere(const FVector2D& Location);
    bool CanPasteNodes();
    void DuplicateNodes();
    bool CanDuplicateNodes();

    FGraphPanelSelectionSet GetSelectedNodes() const;

private:
    void RebuildCustomData();

private:
    TSharedPtr<SCustomNodeEditorWidget> CustomNodeEditorWidget;
    TSharedPtr<SGraphEditor> CustomGraphWidget;
    TSharedPtr<class IDetailsView> PropertyWidget;
    TSharedPtr<class IDetailsView> EditorSettingsWidget;

    TSharedPtr<FUICommandList> GraphEditorCommands;

    UCustomDataAsset* CustomDataAsset;
};