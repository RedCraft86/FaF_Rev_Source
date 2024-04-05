// Copyright (C) RedCraft86. All Rights Reserved.

#include "FaF_Rev_Editor.h"
#include "Interaction/Fields/InteractField.h"
#include "Interaction/Fields/TriggerField.h"
#include "DetailsCustomization/GTActorDetails.h"
#include "GTEditor.h"

#define LOCTEXT_NAMESPACE "FFaF_Rev_EditorModule"

void FFaF_Rev_EditorModule::StartupModule()
{
    if (FPropertyEditorModule* PropertyModule = FModuleManager::LoadModulePtr<FPropertyEditorModule>("PropertyEditor"))
    {
        REGISTER_CLASS_CUSTOMIZATION(PropertyModule, AInteractField, FGTActorDetails);
        REGISTER_CLASS_CUSTOMIZATION(PropertyModule, ATriggerField, FGTActorDetails);
    }
}

void FFaF_Rev_EditorModule::ShutdownModule()
{
    if (FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
    {
        UNREGISTER_CLASS_CUSTOMIZATION(PropertyModule, AInteractField);
        UNREGISTER_CLASS_CUSTOMIZATION(PropertyModule, ATriggerField);
    }
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FFaF_Rev_EditorModule, FaF_Rev_Editor)