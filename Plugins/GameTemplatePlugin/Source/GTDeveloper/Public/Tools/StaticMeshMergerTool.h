// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTDevCommands.h"
#include "Framework/Commands/Commands.h"
#include "Subsystems/EditorActorSubsystem.h"

class FStaticMeshMergerTool
{
public:

	static void Register(const TSharedPtr<FUICommandList>& Commands)
	{
		MAP_TOOL(StaticMeshMerger, FStaticMeshMergerTool);
	}
	
	static void RegisterMenus(const TSharedPtr<FUICommandList>& Commands)
	{
		REGISTER_MENUS(StaticMeshMerger, Actor)
	}

private:

	static void ExecuteAction()
	{
		UEditorActorSubsystem* Subsystem = GEditor ? GEditor->GetEditorSubsystem<UEditorActorSubsystem>() : nullptr;
		if (!Subsystem || (Subsystem && !Subsystem->GetSelectedLevelActors().IsEmpty()))
		{
			FGlobalTabmanager::Get()->TryInvokeTab(FTabId("MergeActors"));
		}
	}
};
