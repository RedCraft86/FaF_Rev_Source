// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "RCRobLibrary.h"
#include "RobAccess.h"

#define OBJECT_TO_TIMER TMap<const void*, TSet<FTimerHandle>>
ROB_DEFINE_VAR(FTimerManager, ObjectToTimers, OBJECT_TO_TIMER);

TArray<FTimerHandle> URCRobLibrary::GetAllWorldTimerHandles(UObject* WorldContext, UObject* OptionalTarget)
{
	TArray<FTimerHandle> Result;
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull))
	{
		OBJECT_TO_TIMER& Timers = World->GetTimerManager().*RobAccess(FTimerManager, ObjectToTimers);

		if (OptionalTarget)
		{
			if (const TSet<FTimerHandle>* FoundTimers = Timers.Find(OptionalTarget))
				Result = FoundTimers->Array();
		}
		else
		{
			for (const TPair<const void*, TSet<FTimerHandle>>& Pair : Timers)
			{
				Result.Append(Pair.Value.Array());
			}
		}
	}

	return Result;
}