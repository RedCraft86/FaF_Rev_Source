// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#include "DataGraphEditor.h"
#include "AssetToolsModule.h"
#include "DataGraphNodeFactory.h"
#include "AssetTypeActions_DataGraph.h"
#include "DataGraphStyle.h"

DEFINE_LOG_CATEGORY(DataGraphEditor);

#define LOCTEXT_NAMESPACE "DataGraphEditor"

void FDataGraphEditorModule::StartupModule()
{
    FDataGraphStyle::Initialize();

    DataGraphNodeFactory = MakeShareable(new FDataGraphNodeFactory());
    if (DataGraphNodeFactory.IsValid())
    {
        FEdGraphUtilities::RegisterVisualNodeFactory(DataGraphNodeFactory);
    }

    if (const FAssetToolsModule* AssetToolsModule = FModuleManager::LoadModulePtr<FAssetToolsModule>("AssetTools"))
    {
        DataGraphAction = MakeShareable(new FAssetTypeActions_DataGraph());
        AssetToolsModule->Get().RegisterAssetTypeActions(DataGraphAction.ToSharedRef());
    }
}

void FDataGraphEditorModule::ShutdownModule()
{
    FDataGraphStyle::Shutdown();
    if (const FAssetToolsModule* AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools"))
    {
        if (DataGraphAction.IsValid()) AssetToolsModule->Get().UnregisterAssetTypeActions(DataGraphAction.ToSharedRef());
    }

    if (DataGraphNodeFactory.IsValid())
    {
        FEdGraphUtilities::UnregisterVisualNodeFactory(DataGraphNodeFactory);
        DataGraphNodeFactory.Reset();
    }
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FDataGraphEditorModule, DataGraphEditor)