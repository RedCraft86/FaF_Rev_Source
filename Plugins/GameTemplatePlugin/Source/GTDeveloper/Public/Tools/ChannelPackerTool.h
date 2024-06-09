// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTDevCommands.h"
#include "Framework/Commands/Commands.h"
#include "Widgets/ChannelPackerWidget.h"

class FChannelPackerTool
{
public:

	static void Register(const TSharedPtr<FUICommandList>& Commands)
	{
		MAP_TOOL(ChannelPacker, FChannelPackerTool);
	}
	
	static void RegisterMenus(const TSharedPtr<FUICommandList>& Commands)
	{
		REGISTER_MENUS(ChannelPacker, General)
	}

	inline static TObjectPtr<UGTChannelPackerWidget> Widget = nullptr;

private:

	static void ExecuteAction();
};
