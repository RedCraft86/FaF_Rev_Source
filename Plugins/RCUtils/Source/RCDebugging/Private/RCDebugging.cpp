// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "RCDebugging.h"

DEFINE_LOG_CATEGORY(RCDebugging);

#define LOCTEXT_NAMESPACE "FRCDebugging"

void FRCDebuggingModule::StartupModule()
{
}

void FRCDebuggingModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRCDebuggingModule, RCDebugging)