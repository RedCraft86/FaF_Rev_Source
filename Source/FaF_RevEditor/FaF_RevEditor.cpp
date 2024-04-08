// Copyright (C) RedCraft86. All Rights Reserved.

#include "FaF_RevEditor.h"
#include "AssetToolsModule.h"
#include "Interaction/Fields/InteractField.h"
#include "Interaction/Fields/TriggerField.h"
#include "DetailsCustomization/GTActorDetails.h"
#include "CustomAssets/DependencyTrackerActions.h"
#include "GTEditor.h"

#define LOCTEXT_NAMESPACE "FFaF_RevEditorModule"

void FFaF_RevEditorModule::StartupModule()
{
    if (FPropertyEditorModule* PropertyModule = FModuleManager::LoadModulePtr<FPropertyEditorModule>("PropertyEditor"))
    {
        REGISTER_CLASS_CUSTOMIZATION(PropertyModule, AInteractField, FGTActorDetails);
        REGISTER_CLASS_CUSTOMIZATION(PropertyModule, ATriggerField, FGTActorDetails);
    }

    if (const FAssetToolsModule* AssetToolsModule = FModuleManager::LoadModulePtr<FAssetToolsModule>("AssetTools"))
    {
        AssetTypeActions.Add(MakeShareable(new FDependencyTrackerActions()));
        for (const TSharedPtr<IAssetTypeActions>& Action : AssetTypeActions)
        {
            AssetToolsModule->Get().RegisterAssetTypeActions(Action.ToSharedRef());
        }
    }
}

void FFaF_RevEditorModule::ShutdownModule()
{
    if (FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
    {
        UNREGISTER_CLASS_CUSTOMIZATION(PropertyModule, AInteractField);
        UNREGISTER_CLASS_CUSTOMIZATION(PropertyModule, ATriggerField);
    }

    if (const FAssetToolsModule* AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools"))
    {
        for (const TSharedPtr<IAssetTypeActions>& Action : AssetTypeActions)
        {
            if (Action.IsValid()) AssetToolsModule->Get().UnregisterAssetTypeActions(Action.ToSharedRef());
        }
    }
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FFaF_RevEditorModule, FaF_Rev_Editor)