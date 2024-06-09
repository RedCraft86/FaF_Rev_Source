// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GTLoadUtilsLibrary.generated.h"

UCLASS(DisplayName = "Load Utils Library")
class GTCORE_API UGTLoadUtilsLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "LoadUtils")
		static void ForceGarbageCollection();

	UFUNCTION(BlueprintCallable, Category = "LoadUtils")
		static bool IsAsyncPackageLoading(const FString Package);
	
	UFUNCTION(BlueprintCallable, Category = "LoadUtils")
		static float GetPackageAsyncLoadPercent(const FString Package);

	UFUNCTION(BlueprintCallable, Category = "LoadUtils", meta = (WorldContext = "WorldContextObject"))
		static void FlushLevelStreaming(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "LoadUtils", meta = (WorldContext = "WorldContextObject"))
		static void ApplyDefaultPriorityLoading(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "LoadUtils", meta = (WorldContext = "WorldContextObject"))
		static void ApplyStreamingPriorityLoading(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "LoadUtils", meta = (WorldContext = "WorldContextObject"))
		static void ApplyHighestPriorityLoading(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "LoadUtils", meta = (WorldContext = "WorldContextObject"))
		static void ApplyCustomPriorityLoading(const UObject* WorldContextObject, bool UseHighPriorityLoading, float MaxAsyncLoadingMilliSeconds, float MaxActorUpdateMilliSeconds);

private:

	static float DefaultLevelStreamingActorsUpdateTimeLimit;
	static float DefaultLevelStreamingComponentsRegistrationGranularity;
	static float DefaultAsyncLoadingTimeLimit;

	static bool HasCapturedDefaults;
	static void CaptureDefaultLoadingSettings();
};
