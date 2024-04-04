// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FFRGameModule final : public IModuleInterface
{
public:
    
	void StartupModule() override;
	void ShutdownModule() override;
};