// Copyright (C) RedCraft86. All Rights Reserved.

#include "AsyncActions/Session/AsyncSessionCheck.h"

UAsyncSessionCheck* UAsyncSessionCheck::CheckSession()
{
	CREATE_TASK
	return Task;
}

void UAsyncSessionCheck::Activate()
{
	CHECK_RUNNING
	GET_SUBSYSTEM
	Subsystem->CheckSession([this](const bool bOpen, const FGamejoltResponse& Response)
	{
		Response.bSuccess ? OnSuccess.Broadcast(bOpen, TEXT("")) : OnFail.Broadcast(bOpen, Response.ErrorMsg);
		FINISH_RUNNING
	});
}
