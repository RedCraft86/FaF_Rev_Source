// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCStaticsLibrary.h"
#include "UserSettings/GCUserSettings.h"
#include "Framework/GCTeleportTarget.h"
#include "Framework/GCGameInstance.h"
#include "Framework/GCGameMode.h"
#include "Player/GCPlayerCharacter.h"
#include "Player/GCPlayerController.h"
#include "GCSettings.h"
#include "EngineUtils.h"

UGCSettings* UGCStaticsLibrary::GetGameCoreSettings()
{
	return UGCSettings::Get();
}

UGCUserSettings* UGCStaticsLibrary::GetUserSettings()
{
	return UGCUserSettings::Get();
}

UGCGameInstance* UGCStaticsLibrary::GetMainGameInstance(const UObject* WorldContextObject)
{
	return UGCGameInstance::Get(WorldContextObject);
}

AGCGameMode* UGCStaticsLibrary::GetMainGameMode(const UObject* WorldContextObject)
{
	return AGCGameMode::Get(WorldContextObject);
}

AGCPlayerController* UGCStaticsLibrary::GetMainPlayerController(const UObject* WorldContextObject)
{
	return AGCPlayerController::Get(WorldContextObject);
}

AGCPlayerCharacter* UGCStaticsLibrary::GetMainPlayerCharacter(const UObject* WorldContextObject)
{
	return AGCPlayerCharacter::Get(WorldContextObject);
}

AGCTeleportTarget* UGCStaticsLibrary::FindTeleporter(const UObject* WorldContextObject, const FGameplayTag InTag)
{
	return AGCTeleportTarget::Get(WorldContextObject, InTag);
}
