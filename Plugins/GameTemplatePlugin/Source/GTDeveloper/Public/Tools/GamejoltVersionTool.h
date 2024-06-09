// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTDevCommands.h"
#include "Framework/Commands/Commands.h"
#include "Widgets/GamejoltVersionWidget.h"

class FGamejoltVersionTool
{
public:

	static void Register(const TSharedPtr<FUICommandList>& Commands)
	{
		MAP_TOOL(GamejoltVersion, FGamejoltVersionTool);
	}
	
	static void RegisterMenus(const TSharedPtr<FUICommandList>& Commands)
	{
		REGISTER_MENUS(GamejoltVersion, General)
	}

	inline static TObjectPtr<UGTGamejoltVersionWidget> Widget = nullptr;

private:

	static void ExecuteAction();
};
