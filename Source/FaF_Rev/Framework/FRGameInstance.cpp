// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRGameInstance.h"
#include "GameSettings/GameSettings.h"
#include "Libraries/GTRuntimeLibrary.h"

void UFRGameInstance::ReloadLevel()
{
	UGTRuntimeLibrary::RestartLevel(this);
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

		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this,
			&UFRGameInstance::ReloadLevel, 0.5f, false);
	}
}
