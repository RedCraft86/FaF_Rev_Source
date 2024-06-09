// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FGTDeveloperModule final : public IModuleInterface
{
public:
	
	void StartupModule() override;
	void ShutdownModule() override;
	
private:

	void RegisterMenus();
	TSharedPtr<FUICommandList> PluginCommands;
};
