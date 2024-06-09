// Copyright (C) RedCraft86. All Rights Reserved.

#include "AsyncActions/Session/AsyncSessionPing.h"

UAsyncSessionPing* UAsyncSessionPing::PingSession(const bool bIsIdling)
{
	CREATE_TASK
	Task->_IsIdling = bIsIdling;
	return Task;
}

void UAsyncSessionPing::Activate()
{
	CHECK_RUNNING
	GET_SUBSYSTEM
	Subsystem->PingSession(_IsIdling, [this](const FGamejoltResponse& Response)
	{
		Response.bSuccess ? OnSuccess.Broadcast(TEXT("")) : OnFail.Broadcast(Response.ErrorMsg);
		FINISH_RUNNING
	});
}
