// Copyright (C) RedCraft86. All Rights Reserved.

#include "AsyncActions/Data/AsyncDataGet.h"

UAsyncDataGet* UAsyncDataGet::GetData(const FString& Key, const bool bUserData)
{
	CREATE_TASK
	Task->_Key = Key;
	Task->_User = bUserData;
	return Task;
}

void UAsyncDataGet::Activate()
{
	CHECK_RUNNING
	GET_SUBSYSTEM
	Subsystem->GetData(_Key, _User, [this](const FGamejoltResponse& Response, const FString& Value)
	{
		Response.bSuccess ? OnSuccess.Broadcast(Value, TEXT("")) : OnFail.Broadcast(Value, Response.ErrorMsg);
		FINISH_RUNNING
	});
}
