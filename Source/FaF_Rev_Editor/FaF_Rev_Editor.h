// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FFaF_Rev_EditorModule final : public IModuleInterface
{
public:
    
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
