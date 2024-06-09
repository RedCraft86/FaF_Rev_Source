// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(DataGraphEditor, All, All);

class FDataGraphEditorModule final : public IModuleInterface
{
public:
    
    void StartupModule() override;
    void ShutdownModule() override;

private:

    TSharedPtr<class IAssetTypeActions> DataGraphAction;
    TSharedPtr<struct FGraphPanelNodeFactory> DataGraphNodeFactory;
};
