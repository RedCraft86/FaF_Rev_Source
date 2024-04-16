// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Helpers/LoggingHelpers.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(FaFRev, All, All);

#define WEAK_THIS WeakThis = TWeakObjectPtr<ThisClass>(this)

#define SMART_LOG(Verbosity, Format) \
	LOG_THIS_LINE(FaFRev, Verbosity); \
	UE_LOG(FaFRev, Verbosity, Format)

#define SMART_LOG_P(Verbosity, Format, ...) \
	LOG_THIS_LINE(FaFRev, Verbosity); \
	UE_LOG(FaFRev, Verbosity, Format, __VA_ARGS__)

class FFRGameModule final : public IModuleInterface
{
public:
    
	void StartupModule() override;
	void ShutdownModule() override;
};