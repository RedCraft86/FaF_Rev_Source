// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTDevCommands.h"
#include "Framework/Commands/Commands.h"
#include "Widgets/TodoTaskListWidget.h"

class FTodoTaskListTool
{
public:
	
	static void Register(const TSharedPtr<FUICommandList>& Commands)
	{
		MAP_TOOL(TodoTaskList, FTodoTaskListTool);
	}
	
	static void RegisterMenus(const TSharedPtr<FUICommandList>& Commands)
	{
		REGISTER_MENUS(TodoTaskList, General)
	}

	inline static TObjectPtr<UGTTodoTaskListWidget> Widget = nullptr;

private:

	static void ExecuteAction();
};
