// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GamejoltSubsystem.h"
#include "AsyncActions/ExtendedAsyncAction.h"

#define GET_SUBSYSTEM UGamejoltSubsystem* Subsystem = UGamejoltSubsystem::Get(); \
	if (!IsValid(Subsystem)) \
	{ \
		UE_KLOG_WARNING(2.0f, TEXT("Cannot get Gamejolt Subsystem on Async Action %s."), *GetClass()->GetName()) \
		return; \
	} \
