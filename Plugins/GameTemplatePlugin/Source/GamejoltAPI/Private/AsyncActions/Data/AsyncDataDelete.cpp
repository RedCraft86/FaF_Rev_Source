// Copyright (C) RedCraft86. All Rights Reserved.

#include "AsyncActions/Data/AsyncDataDelete.h"

UAsyncDataDelete* UAsyncDataDelete::DeleteData(const FString& Key, const bool bUserData)
{
	CREATE_TASK
	Task->_Key = Key;
	Task->_User = bUserData;
	return Task;
}

void UAsyncDataDelete::Activate()
{
	CHECK_RUNNING
	GET_SUBSYSTEM
	Subsystem->DeleteData(_Key, _User, [this](const FGamejoltResponse& Response)
	{
		Response.bSuccess ? OnSuccess.Broadcast(TEXT("")) : OnFail.Broadcast(Response.ErrorMsg);
		FINISH_RUNNING
	});
}
