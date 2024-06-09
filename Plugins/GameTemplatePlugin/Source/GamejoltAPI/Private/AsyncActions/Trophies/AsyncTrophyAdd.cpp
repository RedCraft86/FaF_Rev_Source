// Copyright (C) RedCraft86. All Rights Reserved.

#include "AsyncActions/Trophies/AsyncTrophyAdd.h"

UAsyncTrophyAdd* UAsyncTrophyAdd::AddTrophy(const int32 TrophyID)
{
	CREATE_TASK
	Task->_TrophyID = TrophyID;
	return Task;
}

void UAsyncTrophyAdd::Activate()
{
	CHECK_RUNNING
	GET_SUBSYSTEM
	Subsystem->AddTrophy(_TrophyID, [this](const FGamejoltResponse& Response)
	{
		Response.bSuccess ? OnSuccess.Broadcast(TEXT("")) : OnFail.Broadcast(Response.ErrorMsg);
		FINISH_RUNNING
	});
}
