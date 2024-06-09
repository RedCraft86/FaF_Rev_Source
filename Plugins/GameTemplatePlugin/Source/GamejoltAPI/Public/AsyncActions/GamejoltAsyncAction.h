// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GamejoltSubsystem.h"
#include "AsyncActions/ExtendedAsyncAction.h"
#include "GamejoltAsyncAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGamejoltAsyncActionDelegate, const FString&, Error);

#define GET_SUBSYSTEM UGamejoltSubsystem* Subsystem = UGamejoltSubsystem::Get(); \
	if (!IsValid(Subsystem)) \
	{ \
		UE_KLOG_WARNING(2.0f, TEXT("Cannot get Gamejolt Subsystem on Async Action %s."), *GetClass()->GetName()) \
		return; \
	} \

UCLASS(Abstract)
class GAMEJOLTAPI_API UGamejoltAsyncAction : public UExtendedAsyncAction
{
	GENERATED_BODY()
};
