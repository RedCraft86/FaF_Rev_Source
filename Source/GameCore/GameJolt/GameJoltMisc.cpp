// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "GameJoltMisc.h"
#include "GamejoltAPI.h"

UGJGetDataKeys* UGJGetDataKeys::GetDataKeys()
{
	UGJGetDataKeys* Task = NewObject<UGJGetDataKeys>();
	return Task;
}

void UGJGetDataKeys::Activate()
{
	if (bRunning) return;
	bRunning = true;
	
	GamejoltAPI::GetDataKeys([this](const bool bSuccess, const FString& Response, const TArray<FString>& Keys)
	{
		OnFinish.Broadcast(Keys);
		bRunning = false;
	});
}

UGJRemoveData* UGJRemoveData::RemoveData(const FString Key)
{
	UGJRemoveData* Task = NewObject<UGJRemoveData>();
	Task->DataKey = Key;
	return Task;
}

void UGJRemoveData::Activate()
{
	if (bRunning) return;
	bRunning = true;
	
	GamejoltAPI::DeleteData(DataKey, [this](const bool bSuccess, const FString& Response)
	{
		OnFinish.Broadcast(bSuccess ? TEXT("") : Response);
		bRunning = false;
	});
}
