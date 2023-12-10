// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(RCDebugging, All, All);

class FRCDebuggingModule final : public IModuleInterface
{
	public:

	/** IModuleInterface implementation. */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};