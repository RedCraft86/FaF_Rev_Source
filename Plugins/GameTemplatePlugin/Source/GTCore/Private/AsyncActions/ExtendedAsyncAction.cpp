// Copyright (C) RedCraft86. All Rights Reserved.

#include "AsyncActions/ExtendedAsyncAction.h"

UExtendedAsyncAction::UExtendedAsyncAction()
{
	WorldContext = nullptr;
}

UWorld* UExtendedAsyncAction::GetWorld() const
{
#if WITH_EDITOR
	if (FApp::IsGame())
	{
#endif
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
		if (!IsValid(World))
		{
			UE_KLOG_WARNING(2.0f, TEXT("Cannot access world on Async Action %s. Using last resort method."), *GetClass()->GetName())
			World = GEngine->GetCurrentPlayWorld();
		}

		return World;
#if WITH_EDITOR
	}
	return GEngine->GetCurrentPlayWorld();
#endif
}

bool UExtendedAsyncAction::CheckRunning()
{
	if (bTaskRunning)
	{
		UE_KLOG_WARNING(2.0f, TEXT("Async Action %s is already running. Ignoring new call."), *GetClass()->GetName())
		return true;
	}

	bTaskRunning = true;
	return false;
}
