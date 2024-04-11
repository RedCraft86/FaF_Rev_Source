// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Helpers/LoggingHelpers.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(FaFRev, All, All);

#define SMART_LOG(Verbosity, Format, ...) \
	UE_LOG(FaFRev, Verbosity, TEXT("%s Log: [File: %hs] [Line: %d]"), TEXT(#Verbosity), __FILE__, __LINE__); \
	/* UE_LOG(FaFRev, Verbosity, Format, __VA_ARGS__) */ \
	UE_LOG_KISMET(Verbosity, Format, __VA_ARGS__)

class FFRGameModule final : public IModuleInterface
{
public:
    
	void StartupModule() override;
	void ShutdownModule() override;
};