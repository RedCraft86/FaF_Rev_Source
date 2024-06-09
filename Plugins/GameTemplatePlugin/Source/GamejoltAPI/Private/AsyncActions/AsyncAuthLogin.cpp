// Copyright (C) RedCraft86. All Rights Reserved.

#include "AsyncActions/AsyncAuthLogin.h"

UAsyncAuthLogin* UAsyncAuthLogin::AuthLogin(const FString& UserName, const FString& UserToken, const bool bRemember)
{
	CREATE_TASK
	Task->_UserName = UserName;
	Task->_UserToken = UserToken;
	Task->_Remember = bRemember;
	return Task;
}

void UAsyncAuthLogin::Activate()
{
	CHECK_RUNNING
	GET_SUBSYSTEM
	Subsystem->AuthLogin(_UserName, _UserToken, _Remember, [this](const FGamejoltResponse& Response)
	{
		Response.bSuccess ? OnSuccess.Broadcast(TEXT("")) : OnFail.Broadcast(Response.ErrorMsg);
		FINISH_RUNNING
	});
}

void UAsyncAuthLogin::Logout(const bool bForget)
{
	if (UGamejoltSubsystem* Subsystem = UGamejoltSubsystem::Get())
	{
		Subsystem->Logout(bForget);
	}
}
