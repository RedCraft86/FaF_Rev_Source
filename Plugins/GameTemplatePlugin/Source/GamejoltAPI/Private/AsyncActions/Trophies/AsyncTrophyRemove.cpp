// Copyright (C) RedCraft86. All Rights Reserved.

#include "AsyncActions/Trophies/AsyncTrophyRemove.h"

UAsyncTrophyRemove* UAsyncTrophyRemove::RemoveTrophy(const int32 TrophyID)
{
	CREATE_TASK
	Task->_TrophyID = TrophyID;
	return Task;
}

void UAsyncTrophyRemove::Activate()
{
	CHECK_RUNNING
	GET_SUBSYSTEM
	Subsystem->RemoveTrophy(_TrophyID, [this](const FGamejoltResponse& Response)
	{
		Response.bSuccess ? OnSuccess.Broadcast(TEXT("")) : OnFail.Broadcast(Response.ErrorMsg);
		FINISH_RUNNING
	});
}
