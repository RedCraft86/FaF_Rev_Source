// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "RCUtilsSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RCGlobalUtilsLibrary.generated.h"

struct FGameplayTag;
struct FInstancedStruct;

UCLASS(DisplayName = "Global Utils Library")
class RCRUNTIME_API URCGlobalUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	/* Sets a global variable.
	* @param bLevelVariable - Non persistent, this global variable will be removed on map change.
	*/
	UFUNCTION(BlueprintCallable, Category = "GlobalUtils|Variables", meta = (WorldContext = "WorldContextObject"))
		static void SetGlobalVariable(const UObject* WorldContextObject, const FGameplayTag Key, const FInstancedStruct& Value, const bool bLevelVariable);

	/* Gets a global variable. */
	UFUNCTION(BlueprintCallable, Category = "GlobalUtils|Variables", meta = (WorldContext = "WorldContextObject"))
		static FInstancedStruct GetGlobalVariable(const UObject* WorldContextObject, const FGameplayTag Key);

	/* Removes a global variable. */
	UFUNCTION(BlueprintCallable, Category = "GlobalUtils|Variables", meta = (WorldContext = "WorldContextObject"))
		static void RemoveGlobalVariable(const UObject* WorldContextObject, const FGameplayTag Key);

	/* Clears all global variables. */
	UFUNCTION(BlueprintCallable, Category = "GlobalUtils|Variables", meta = (WorldContext = "WorldContextObject"))
		static void RemoveAllGlobalVariables(const UObject* WorldContextObject);

	/* Binds a local event to a global event. */
	UFUNCTION(BlueprintCallable, Category = "GlobalUtils|Events", meta = (WorldContext = "WorldContextObject"))
		static void BindToGlobalEvent(const UObject* WorldContextObject, const FGameplayTag Key, const FRCGlobalEventSingle& Delegate);

	/* Unbinds a local event from a global event. */
	UFUNCTION(BlueprintCallable, Category = "GlobalUtils|Events", meta = (WorldContext = "WorldContextObject"))
		static void UnbindFromGlobalEvent(const UObject* WorldContextObject, const FGameplayTag Key, const FRCGlobalEventSingle& Delegate);

	/* Invokes (Calls) a global event. */
	UFUNCTION(BlueprintCallable, Category = "GlobalUtils|Events", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "Caller"))
		static void InvokeGlobalEvent(const UObject* WorldContextObject, UObject* Caller, const FGameplayTag Key, const FInstancedStruct& Payload);

	/* Removes a global events. */
	UFUNCTION(BlueprintCallable, Category = "GlobalUtils|Events", meta = (WorldContext = "WorldContextObject"))
		static void RemoveGlobalEvent(const UObject* WorldContextObject, const FGameplayTag Key);

	/* Clears all global events. */
	UFUNCTION(BlueprintCallable, Category = "GlobalUtils|Events", meta = (WorldContext = "WorldContextObject"))
		static void RemoveAllGlobalEvents(const UObject* WorldContextObject);
};
