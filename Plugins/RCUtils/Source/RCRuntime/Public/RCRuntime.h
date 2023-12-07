// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(RCRuntime, All, All);

class FRCRuntimeModule final : public IModuleInterface
{
public:

	/* IModuleInterface implementation. */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};