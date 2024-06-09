// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(GTRuntime, All, All);

class FGTRuntimeModule final : public IModuleInterface
{
public:
    
	void StartupModule() override;
	void ShutdownModule() override;
};
