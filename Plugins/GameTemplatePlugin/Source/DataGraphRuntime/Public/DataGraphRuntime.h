// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(DataGraphRuntime, All, All);

class FDataGraphRuntimeModule final : public IModuleInterface
{
public:
    
    void StartupModule() override;
    void ShutdownModule() override;
};
