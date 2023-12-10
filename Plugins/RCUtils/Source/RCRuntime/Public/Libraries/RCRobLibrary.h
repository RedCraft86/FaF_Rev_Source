// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "RCRobLibrary.generated.h"

UCLASS(DisplayName = "Rob Function Library")
class RCRUNTIME_API URCRobLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	/* Gets all the timer handles being managed by FTimerManager by robbing a private property
	* @param OptionalTarget - Optional, to filter timers from specific objects
	* @return Array of all found timers
	*/
	UFUNCTION(BlueprintCallable, Category = "RobAccess", meta = (WorldContext = "WorldContext", AdvancedDisplay = "OptionalTarget"))
		static TArray<FTimerHandle> GetAllWorldTimerHandles(UObject* WorldContext, UObject* OptionalTarget);
};
