// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "Classes/LowEntryExtendedStandardLibrary.h"

using ULowEntryLibrary = ULowEntryExtendedStandardLibrary;

class FGTCoreModule final : public IModuleInterface
{
public:
	
	void StartupModule() override;
	void ShutdownModule() override;
};
