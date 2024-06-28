// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTDevStyle.h"

#define MAP_TOOL(Tool, ThisClass) \
	Commands->MapAction(FGTDevCommands::Get().Tool, FExecuteAction::CreateStatic(&ThisClass::ExecuteAction), FCanExecuteAction());

#define REGISTER_MENUS(Tool, Category) \
	if (UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window")) \
	{ \
		FToolMenuSection& Section = Menu->FindOrAddSection("WindowButton_GTTools"#Category); \
		Section.AddMenuEntryWithCommandList(FGTDevCommands::Get().Tool, Commands); \
	} \
	if (UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar")) \
	{ \
		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("ToolbarButton_GTTools"#Category); \
		Section.AddEntry(FToolMenuEntry::InitToolBarButton(FGTDevCommands::Get().Tool)).SetCommandList(Commands); \
	}

#define LOCTEXT_NAMESPACE "GTDeveloper"
class FGTDevCommands final : public TCommands<FGTDevCommands>
{
public:

	FGTDevCommands()
		: TCommands(TEXT("GTDevTools"), LOCTEXT("Contexts", "GTDevTools"),
			NAME_None, FGTDevStyle::GetStyleSetName())
	{}

	virtual void RegisterCommands() override
	{
		UI_COMMAND(RestartEditor, "Restart Editor", "Restarts the Unreal Editor",
			EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control | EModifierKey::Shift, EKeys::R));
		
		UI_COMMAND(ChannelPacker, "Channel Packer", "Opens the Texture Channel Editor",
			EUserInterfaceActionType::Button, FInputChord());

		UI_COMMAND(TodoTaskList, "Todo List", "Opens the Todo/Task List",
			EUserInterfaceActionType::Button, FInputChord());

		UI_COMMAND(StaticMeshMerger, "Merge Static Meshes", "Opens the Merge Actors tab",
			EUserInterfaceActionType::Button, FInputChord());

		UI_COMMAND(StaticMeshBaker, "Bake Static Meshes", "Bake static meshes from a single blueprint to individual static meshes",
			EUserInterfaceActionType::Button, FInputChord());

		UI_COMMAND(StaticMeshInstancer, "Instance Static Meshes", "Converts static meshes into Instanced Static Meshes",
			EUserInterfaceActionType::Button, FInputChord());
	}
	
	TSharedPtr<FUICommandInfo> RestartEditor;
	TSharedPtr<FUICommandInfo> ChannelPacker;
	TSharedPtr<FUICommandInfo> TodoTaskList;
	TSharedPtr<FUICommandInfo> StaticMeshMerger;
	TSharedPtr<FUICommandInfo> StaticMeshBaker;
	TSharedPtr<FUICommandInfo> StaticMeshInstancer;
};
#undef LOCTEXT_NAMESPACE