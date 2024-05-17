// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRGameInstance.h"
#include "GameSettings/GameSettings.h"
#include "Libraries/GTRuntimeLibrary.h"

UFRGameInstance::UFRGameInstance() : bRanFirstLoads(false), bPlayerInvincible(false)
{
}

void UFRGameInstance::ReloadLevel() const
{
	UGTRuntimeLibrary::RestartLevel(this);
}

void UFRGameInstance::SetPlayerInvincible(bool bInInvincible)
{
	if (bPlayerInvincible != bInInvincible)
	{
		bPlayerInvincible = bInInvincible;
		OnInvincibilityUpdated.Broadcast(bPlayerInvincible);
	}
}

void UFRGameInstance::Init()
{
	Super::Init();
	UGameSettings::Get()->GameInstance = this;
}

void UFRGameInstance::OnWorldBeginPlay(UWorld* InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	UGameSettings::Get()->GameInstance = this;
	if (!bRanFirstLoads)
	{
		bRanFirstLoads = true;
		UGameSettings::Get()->InitializeSettings();
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UFRGameInstance::ReloadLevel);
	}
}
