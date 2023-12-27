// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCGameMode.h"
#include "GCGameInstance.h"

AGCGameMode::AGCGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

AGCGameMode* AGCGameMode::Get(const UObject* WorldContext)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	return World ? World->GetAuthGameMode<AGCGameMode>() : nullptr;
}