// Copyright (C) RedCraft86. All Rights Reserved.

#include "FaF_RevEditor.h"
#include "AssetToolsModule.h"
#include "DetailsCustomization/GTActorDetails.h"
#include "GTEditor.h"

#include "Framework/FRPlayer.h"
#include "Interaction/Fields/InteractField.h"
#include "Interaction/Fields/TriggerField.h"
#include "Inventory/InventoryItemActor.h"
#include "Inventory/InventoryPreview.h"
#include "PostProcess/SmartPostProcess.h"
#include "Electronics/ElectricActor.h"
#include "Electronics/ElectricSwitch.h"

#define LOCTEXT_NAMESPACE "FFaF_RevEditorModule"

void FFaF_RevEditorModule::StartupModule()
{
    if (FPropertyEditorModule* PropertyModule = FModuleManager::LoadModulePtr<FPropertyEditorModule>("PropertyEditor"))
    {
        REGISTER_CLASS_CUSTOMIZATION(PropertyModule, AFRPlayerBase, FGTActorDetails);
        REGISTER_CLASS_CUSTOMIZATION(PropertyModule, AInventoryPreview, FGTActorDetails);
        REGISTER_CLASS_CUSTOMIZATION(PropertyModule, ASmartPostProcess, FGTActorDetails);
        REGISTER_CLASS_CUSTOMIZATION(PropertyModule, AElectricActorBase, FGTActorDetails);
    }

    if (const FAssetToolsModule* AssetToolsModule = FModuleManager::LoadModulePtr<FAssetToolsModule>("AssetTools"))
    {
        //AssetTypeActions.Add(MakeShareable(new FGameMusicDataActions()));
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
        UNREGISTER_CLASS_CUSTOMIZATION(PropertyModule, AFRPlayerBase);
        UNREGISTER_CLASS_CUSTOMIZATION(PropertyModule, AInventoryPreview);
        UNREGISTER_CLASS_CUSTOMIZATION(PropertyModule, ASmartPostProcess);
        UNREGISTER_CLASS_CUSTOMIZATION(PropertyModule, AElectricActorBase);
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