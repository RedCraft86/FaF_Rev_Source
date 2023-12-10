// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "GameVersion.h"
#include "GamejoltAPI.h"

UGJSetLatestVersion* UGJSetLatestVersion::SetLatestVersion(const FRCGameVersion NewVersion)
{
	UGJSetLatestVersion* Task = NewObject<UGJSetLatestVersion>();
	Task->Version = NewVersion;
	return Task;
}

void UGJSetLatestVersion::Activate()
{
	if (bRunning) return;
	bRunning = true;
	
	GamejoltAPI::SetLatestVersion(Version, [this](const bool bSuccess, const FString& Response)
	{
		if (bSuccess) OnSuccess.Broadcast(TEXT("")); else OnFail.Broadcast(Response);
		bRunning = false;
	});
}

UGJGetLatestVersion* UGJGetLatestVersion::GetLatestVersion()
{
	UGJGetLatestVersion* Task = NewObject<UGJGetLatestVersion>();
	return Task;
}

void UGJGetLatestVersion::Activate()
{
	if (bRunning) return;
	bRunning = true;
	
	GamejoltAPI::GetLatestVersion([this](const FRCGameVersion& Version)
	{
		OnFinished.Broadcast(Version);
		bRunning = false;
	});
}
