// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FGTCoreModule final : public IModuleInterface
{
public:
	
	void StartupModule() override;
	void ShutdownModule() override;
};
