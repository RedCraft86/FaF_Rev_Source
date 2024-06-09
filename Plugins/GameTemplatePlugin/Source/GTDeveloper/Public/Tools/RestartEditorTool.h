// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTDevCommands.h"
#include "Framework/Commands/Commands.h"

class FRestartEditorTool
{
public:

	static void Register(const TSharedPtr<FUICommandList>& Commands)
	{
		MAP_TOOL(RestartEditor, FRestartEditorTool);
	}
	
	static void RegisterMenus(const TSharedPtr<FUICommandList>& Commands)
	{
		REGISTER_MENUS(RestartEditor, General)
	}

private:

	static void ExecuteAction()
	{
		FUnrealEdMisc::Get().RestartEditor(false);
	}
};
