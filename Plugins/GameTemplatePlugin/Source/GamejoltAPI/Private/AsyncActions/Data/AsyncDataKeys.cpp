// Copyright (C) RedCraft86. All Rights Reserved.

#include "AsyncActions/Data/AsyncDataKeys.h"

UAsyncDataKeys* UAsyncDataKeys::GetDataKeys(const bool bUserData)
{
	CREATE_TASK
	Task->_User = bUserData;
	return Task;
}

void UAsyncDataKeys::Activate()
{
	CHECK_RUNNING
	GET_SUBSYSTEM
	Subsystem->GetDataKeys(_User, [this](const FGamejoltResponse& Response, const TArray<FString>& Keys)
	{
		Response.bSuccess ? OnSuccess.Broadcast(Keys, TEXT("")) : OnFail.Broadcast(Keys, Response.ErrorMsg);
		FINISH_RUNNING
	});
}
