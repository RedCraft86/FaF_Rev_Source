// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Helpers/LoggingHelpers.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(FaFRev, All, All);

#define SMART_LOG(Verbosity, Format) \
	LOG_THIS_LINE(FaFRev, Verbosity); \
	UE_LOG(FaFRev, Verbosity, Format)

#define SMART_LOG_P(Verbosity, Format, ...) \
	LOG_THIS_LINE(FaFRev, Verbosity); \
	UE_LOG(FaFRev, Verbosity, Format, __VA_ARGS__)
	
#define WEAK_THIS WeakThis = TWeakObjectPtr<ThisClass>(this)
#define SETUP_PULLDOWN(ThisStruct) \
	ThisStruct() { SelectedValue = NAME_None; } \
	ThisStruct(const FName& InValue) { SelectedValue = InValue; } \
	friend FArchive& operator<<(FArchive& Ar, ThisStruct& Pulldown) \
	{ \
		Ar << Pulldown.SelectedValue; \
		return Ar; \
	} \
	void Set(const FName& InValue) { SelectedValue = InValue; } \
	bool IsValid() const { return !IsNone(); } 

class FFRGameModule final : public IModuleInterface
{
public:
    
	void StartupModule() override;
	void ShutdownModule() override;
};