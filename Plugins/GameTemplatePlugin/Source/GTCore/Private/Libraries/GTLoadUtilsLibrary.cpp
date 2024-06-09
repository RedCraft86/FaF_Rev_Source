// Copyright (C) RedCraft86. All Rights Reserved.

#include "Libraries/GTLoadUtilsLibrary.h"
#include "Engine/CoreSettings.h"

float UGTLoadUtilsLibrary::DefaultLevelStreamingComponentsRegistrationGranularity;
float UGTLoadUtilsLibrary::DefaultLevelStreamingActorsUpdateTimeLimit;
float UGTLoadUtilsLibrary::DefaultAsyncLoadingTimeLimit;
bool UGTLoadUtilsLibrary::HasCapturedDefaults = false;

void UGTLoadUtilsLibrary::ForceGarbageCollection()
{
	if (GEngine)
	{
#if WITH_EDITOR
		GEngine->ForceGarbageCollection(false);
#else
		GEngine->ForceGarbageCollection(true);
#endif
	}
}

bool UGTLoadUtilsLibrary::IsAsyncPackageLoading(const FString Package)
{
	return GetAsyncLoadPercentage(*Package) >= 0.0f;
}

float UGTLoadUtilsLibrary::GetPackageAsyncLoadPercent(const FString Package)
{
	return GetAsyncLoadPercentage(*Package);
}

void UGTLoadUtilsLibrary::FlushLevelStreaming(const UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		GEngine->BlockTillLevelStreamingCompleted(World);
	}
}

void UGTLoadUtilsLibrary::ApplyDefaultPriorityLoading(const UObject* WorldContextObject)
{
	CaptureDefaultLoadingSettings();
	ApplyCustomPriorityLoading(WorldContextObject, false,
		DefaultAsyncLoadingTimeLimit, DefaultLevelStreamingActorsUpdateTimeLimit);
}

void UGTLoadUtilsLibrary::ApplyStreamingPriorityLoading(const UObject* WorldContextObject)
{
	ApplyCustomPriorityLoading(WorldContextObject, false, 10.0f, 10.0f);
}

void UGTLoadUtilsLibrary::ApplyHighestPriorityLoading(const UObject* WorldContextObject)
{
	ApplyCustomPriorityLoading(WorldContextObject, true, FLT_MAX, FLT_MAX);
}

void UGTLoadUtilsLibrary::ApplyCustomPriorityLoading(const UObject* WorldContextObject, const bool UseHighPriorityLoading,
	const float MaxAsyncLoadingMilliSeconds, const float MaxActorUpdateMilliSeconds)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (AWorldSettings* WorldSettings = World->GetWorldSettings())
		{
			GLevelStreamingComponentsRegistrationGranularity = DefaultLevelStreamingComponentsRegistrationGranularity;
			WorldSettings->bHighPriorityLoadingLocal = UseHighPriorityLoading;
			GLevelStreamingActorsUpdateTimeLimit = MaxActorUpdateMilliSeconds;
			GAsyncLoadingUseFullTimeLimit = UseHighPriorityLoading;
			GAsyncLoadingTimeLimit = MaxAsyncLoadingMilliSeconds;
		}
	}
}

void UGTLoadUtilsLibrary::CaptureDefaultLoadingSettings()
{
	if (!HasCapturedDefaults)
	{
		DefaultLevelStreamingComponentsRegistrationGranularity = GLevelStreamingComponentsRegistrationGranularity;
		DefaultLevelStreamingActorsUpdateTimeLimit = GLevelStreamingActorsUpdateTimeLimit;
		DefaultAsyncLoadingTimeLimit = GAsyncLoadingTimeLimit;
		HasCapturedDefaults = true;
	}
}
