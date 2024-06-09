// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTDevCommands.h"
#include "Framework/Commands/Commands.h"

class FStaticMeshBakerTool
{
public:

	static void Register(const TSharedPtr<FUICommandList>& Commands)
	{
		MAP_TOOL(StaticMeshBaker, FStaticMeshBakerTool);
	}
	
	static void RegisterMenus(const TSharedPtr<FUICommandList>& Commands)
	{
		REGISTER_MENUS(StaticMeshBaker, Actor)
	}

private:

	static void ExecuteAction();
};
