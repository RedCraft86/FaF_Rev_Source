// Copyright (C) RedCraft86. All Rights Reserved.

#include "AsyncActions/Data/AsyncDataSet.h"

UAsyncDataSet* UAsyncDataSet::SetData(const FString& Key, const FString& Value, const bool bUserData)
{
	CREATE_TASK
	Task->_Key = Key;
	Task->_Value = Value;
	Task->_User = bUserData;
	return Task;
}

void UAsyncDataSet::Activate()
{
	CHECK_RUNNING
	GET_SUBSYSTEM
	Subsystem->SetData(_Key, _Value, _User, [this](const FGamejoltResponse& Response)
	{
		Response.bSuccess ? OnSuccess.Broadcast(TEXT("")) : OnFail.Broadcast(Response.ErrorMsg);
		FINISH_RUNNING
	});
}
