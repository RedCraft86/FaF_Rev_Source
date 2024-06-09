// Copyright (C) RedCraft86. All Rights Reserved.

#include "AsyncActions/Session/AsyncSessionClose.h"

UAsyncSessionClose* UAsyncSessionClose::CloseSession()
{
	CREATE_TASK
	return Task;
}

void UAsyncSessionClose::Activate()
{
	CHECK_RUNNING
	GET_SUBSYSTEM
	Subsystem->CloseSession([this](const FGamejoltResponse& Response)
	{
		Response.bSuccess ? OnSuccess.Broadcast(TEXT("")) : OnFail.Broadcast(Response.ErrorMsg);
		FINISH_RUNNING
	});
}
