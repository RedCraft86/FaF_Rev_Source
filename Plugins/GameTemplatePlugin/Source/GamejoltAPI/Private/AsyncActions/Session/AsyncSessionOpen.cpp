// Copyright (C) RedCraft86. All Rights Reserved.

#include "AsyncActions/Session/AsyncSessionOpen.h"

UAsyncSessionOpen* UAsyncSessionOpen::OpenSession()
{
	CREATE_TASK
	return Task;
}

void UAsyncSessionOpen::Activate()
{
	CHECK_RUNNING
	GET_SUBSYSTEM
	Subsystem->OpenSession([this](const FGamejoltResponse& Response)
	{
		Response.bSuccess ? OnSuccess.Broadcast(TEXT("")) : OnFail.Broadcast(Response.ErrorMsg);
		FINISH_RUNNING
	});
}
