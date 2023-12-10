// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(RCEditor, All, All);

class FRCEditorModule final : public IModuleInterface
{
public:

	/* IModuleInterface implementation. */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};