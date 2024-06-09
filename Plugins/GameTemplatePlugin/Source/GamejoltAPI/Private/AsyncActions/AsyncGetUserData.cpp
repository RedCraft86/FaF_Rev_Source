// Copyright (C) RedCraft86. All Rights Reserved.

#include "AsyncActions/AsyncGetUserData.h"

UAsyncGetUserData* UAsyncGetUserData::GetUserData()
{
	CREATE_TASK
	return Task;
}

void UAsyncGetUserData::Activate()
{
	CHECK_RUNNING
	GET_SUBSYSTEM
	Subsystem->GetUserData([this](const FGamejoltResponse& Response, const FGamejoltUserData& Data)
	{
		Response.bSuccess ? OnSuccess.Broadcast(Data, TEXT("")) : OnFail.Broadcast(Data, Response.ErrorMsg);
		FINISH_RUNNING
	});
}
