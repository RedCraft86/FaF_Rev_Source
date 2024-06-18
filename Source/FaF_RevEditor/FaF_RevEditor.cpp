// Copyright (C) RedCraft86. All Rights Reserved.

#include "FaF_RevEditor.h"
#include "AssetToolsModule.h"
#include "DetailsCustomization/GTActorDetails.h"
#include "GTEditor.h"

#include "Framework/FRPlayer.h"
#include "Interaction/Actors/FRDoorActor.h"
#include "Inventory/InventoryPreview.h"
#include "PostProcess/SmartPostProcess.h"

#define LOCTEXT_NAMESPACE "FFaF_RevEditorModule"

void FFaF_RevEditorModule::StartupModule()
{
    // Backup check
    {
        const FString LockPath = FPaths::ProjectContentDir() / TEXT("Backup.Lock");
        if (FPaths::FileExists(LockPath))
        {
            FArchive* LockFile = IFileManager::Get().CreateFileWriter(*LockPath);
            checkf(LockFile, TEXT("Please wait while the project is being backed up."))
            if (LockFile) LockFile->Close();
        }
    }
    
    if (FPropertyEditorModule* PropertyModule = FModuleManager::LoadModulePtr<FPropertyEditorModule>("PropertyEditor"))
    {
        REGISTER_CLASS_CUSTOMIZATION(AFRPlayerBase, FGTActorDetails);
        REGISTER_CLASS_CUSTOMIZATION(AFRDoorBase, FGTActorDetails);
        REGISTER_CLASS_CUSTOMIZATION(AInventoryPreview, FGTActorDetails);
        REGISTER_CLASS_CUSTOMIZATION(ASmartPostProcess, FGTActorDetails);
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
        UNREGISTER_CLASS_CUSTOMIZATION(AFRPlayerBase);
        UNREGISTER_CLASS_CUSTOMIZATION(AFRDoorBase);
        UNREGISTER_CLASS_CUSTOMIZATION(AInventoryPreview);
        UNREGISTER_CLASS_CUSTOMIZATION(ASmartPostProcess);
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