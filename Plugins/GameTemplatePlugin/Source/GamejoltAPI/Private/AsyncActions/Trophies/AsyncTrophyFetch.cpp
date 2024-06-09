// Copyright (C) RedCraft86. All Rights Reserved.

#include "AsyncActions/Trophies/AsyncTrophyFetch.h"

UAsyncTrophyFetch* UAsyncTrophyFetch::FetchTrophies(const EGamejoltTrophyFilter TypeFilter, const TArray<int32>& ID_Filter)
{
	CREATE_TASK
	Task->_TypeFilter = TypeFilter;
	Task->_IDFilter = ID_Filter;
	return Task;
}

void UAsyncTrophyFetch::Activate()
{
	CHECK_RUNNING
	GET_SUBSYSTEM
	Subsystem->FetchTrophies(_TypeFilter, _IDFilter, [this](const FGamejoltResponse& Response, const TArray<FGamejoltTrophyData>& Trophies)
	{
		Response.bSuccess ? OnSuccess.Broadcast(Trophies, TEXT("")) : OnFail.Broadcast(Trophies, Response.ErrorMsg);
		FINISH_RUNNING
	});
}
