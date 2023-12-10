// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "RCLoadUtilsLibrary.h"
#include "Engine/CoreSettings.h"
#include "GameFramework/WorldSettings.h"

float URCLoadUtilsLibrary::DefaultLevelStreamingComponentsRegistrationGranularity;
float URCLoadUtilsLibrary::DefaultLevelStreamingActorsUpdateTimeLimit;
float URCLoadUtilsLibrary::DefaultAsyncLoadingTimeLimit;
bool URCLoadUtilsLibrary::HasCapturedDefaults = false;

void URCLoadUtilsLibrary::ForceGarbageCollection()
{
#if WITH_EDITOR
	GEngine->ForceGarbageCollection(false);
#else
	GEngine->ForceGarbageCollection(true);
#endif
}

void URCLoadUtilsLibrary::FlushLevelStreaming(const UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		GEngine->BlockTillLevelStreamingCompleted(World);
	}
}

void URCLoadUtilsLibrary::ApplyDefaultPriorityLoading(const UObject* WorldContextObject)
{
	CaptureDefaultLoadingSettings();
	ApplyCustomPriorityLoading(WorldContextObject, false, DefaultAsyncLoadingTimeLimit, DefaultLevelStreamingActorsUpdateTimeLimit);
}

void URCLoadUtilsLibrary::ApplyStreamingPriorityLoading(const UObject* WorldContextObject)
{
	ApplyCustomPriorityLoading(WorldContextObject, false, 10.0f, 10.0f);
}

void URCLoadUtilsLibrary::ApplyHighestPriorityLoading(const UObject* WorldContextObject)
{
	ApplyCustomPriorityLoading(WorldContextObject, true, FLT_MAX, FLT_MAX);
}

void URCLoadUtilsLibrary::ApplyCustomPriorityLoading(const UObject* WorldContextObject, bool UseHighPriorityLoading, float MaxAsyncLoadingMilliSeconds, float MaxActorUpdateMilliSeconds)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (AWorldSettings* WorldSettings = World->GetWorldSettings())
		{
			WorldSettings->bHighPriorityLoadingLocal = UseHighPriorityLoading;
			GLevelStreamingActorsUpdateTimeLimit = MaxActorUpdateMilliSeconds;
			GLevelStreamingComponentsRegistrationGranularity = DefaultLevelStreamingComponentsRegistrationGranularity;
			GAsyncLoadingUseFullTimeLimit = UseHighPriorityLoading;
			GAsyncLoadingTimeLimit = MaxAsyncLoadingMilliSeconds;
		}
	}
}

void URCLoadUtilsLibrary::CaptureDefaultLoadingSettings()
{
	if (!HasCapturedDefaults)
	{
		DefaultLevelStreamingComponentsRegistrationGranularity = GLevelStreamingComponentsRegistrationGranularity;
		DefaultLevelStreamingActorsUpdateTimeLimit = GLevelStreamingActorsUpdateTimeLimit;
		DefaultAsyncLoadingTimeLimit = GAsyncLoadingTimeLimit;
		HasCapturedDefaults = true;
	}
}
