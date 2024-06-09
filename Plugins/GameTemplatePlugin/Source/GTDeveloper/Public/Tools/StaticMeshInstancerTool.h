// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTDevCommands.h"
#include "Framework/Commands/Commands.h"

class FStaticMeshInstancerTool
{
public:

	static void Register(const TSharedPtr<FUICommandList>& Commands)
	{
		MAP_TOOL(StaticMeshInstancer, FStaticMeshInstancerTool);
	}
	
	static void RegisterMenus(const TSharedPtr<FUICommandList>& Commands)
	{
		REGISTER_MENUS(StaticMeshInstancer, Actor)
	}

private:

	static void ExecuteAction();
};
