// Copyright (C) RedCraft86. All Rights Reserved.

#include "GTDeveloper.h"
#include "Interfaces/IMainFrameModule.h"
#include "Tools/StaticMeshInstancerTool.h"
#include "Tools/StaticMeshMergerTool.h"
#include "Tools/StaticMeshBakerTool.h"
#include "Tools/RestartEditorTool.h"
#include "Tools/ChannelPackerTool.h"
#include "Tools/TodoTaskListTool.h"
#include "GTDevCommands.h"
#include "GTDevStyle.h"

#define LOCTEXT_NAMESPACE "GTDeveloper"

void FGTDeveloperModule::StartupModule()
{
    FGTDevStyle::Initialize();
    FGTDevCommands::Register();
    
    PluginCommands = MakeShareable(new FUICommandList);
    FRestartEditorTool::Register(PluginCommands);
    FChannelPackerTool::Register(PluginCommands);
    FTodoTaskListTool::Register(PluginCommands);
    FStaticMeshMergerTool::Register(PluginCommands);
    FStaticMeshBakerTool::Register(PluginCommands);
    FStaticMeshInstancerTool::Register(PluginCommands);

    IMainFrameModule& MainFrame = FModuleManager::Get().LoadModuleChecked<IMainFrameModule>("MainFrame");
    MainFrame.GetMainFrameCommandBindings()->Append(PluginCommands.ToSharedRef());

    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(
        this, &FGTDeveloperModule::RegisterMenus));
}

void FGTDeveloperModule::ShutdownModule()
{
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
    FGTDevCommands::Unregister();
    FGTDevStyle::Shutdown();
}

void FGTDeveloperModule::RegisterMenus()
{
    FToolMenuOwnerScoped OwnerScoped(this);
    {
        FRestartEditorTool::RegisterMenus(PluginCommands);
        FChannelPackerTool::RegisterMenus(PluginCommands);
        FTodoTaskListTool::RegisterMenus(PluginCommands);
        FStaticMeshMergerTool::RegisterMenus(PluginCommands);
        FStaticMeshBakerTool::RegisterMenus(PluginCommands);
        FStaticMeshInstancerTool::RegisterMenus(PluginCommands);
    }
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FGTDeveloperModule, GTDeveloper)