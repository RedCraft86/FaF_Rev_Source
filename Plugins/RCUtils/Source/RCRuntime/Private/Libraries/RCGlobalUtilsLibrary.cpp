// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "RCGlobalUtilsLibrary.h"

void URCGlobalUtilsLibrary::SetGlobalVariable(const UObject* WorldContextObject, const FGameplayTag Key, const FInstancedStruct& Value, const bool bLevelVariable)
{
	if (URCUtilsSubsystem* Subsystem = URCUtilsSubsystem::Get(WorldContextObject))
		Subsystem->SetGlobalVariable(Key, Value, bLevelVariable);
}

FInstancedStruct URCGlobalUtilsLibrary::GetGlobalVariable(const UObject* WorldContextObject, const FGameplayTag Key)
{
	if (URCUtilsSubsystem* Subsystem = URCUtilsSubsystem::Get(WorldContextObject))
		return Subsystem->GetGlobalVariable(Key);

	return FInstancedStruct();
}

void URCGlobalUtilsLibrary::RemoveGlobalVariable(const UObject* WorldContextObject, const FGameplayTag Key)
{
	if (URCUtilsSubsystem* Subsystem = URCUtilsSubsystem::Get(WorldContextObject))
		Subsystem->RemoveGlobalVariable(Key);
}

void URCGlobalUtilsLibrary::RemoveAllGlobalVariables(const UObject* WorldContextObject)
{
	if (URCUtilsSubsystem* Subsystem = URCUtilsSubsystem::Get(WorldContextObject))
		Subsystem->RemoveAllGlobalVariables();
}

void URCGlobalUtilsLibrary::BindToGlobalEvent(const UObject* WorldContextObject, const FGameplayTag Key, const FRCGlobalEventSingle& Delegate)
{
	if (URCUtilsSubsystem* Subsystem = URCUtilsSubsystem::Get(WorldContextObject))
		Subsystem->BindToGlobalEvent(Key, Delegate);
}

void URCGlobalUtilsLibrary::UnbindFromGlobalEvent(const UObject* WorldContextObject, const FGameplayTag Key, const FRCGlobalEventSingle& Delegate)
{
	if (URCUtilsSubsystem* Subsystem = URCUtilsSubsystem::Get(WorldContextObject))
		Subsystem->UnbindFromGlobalEvent(Key, Delegate);
}

void URCGlobalUtilsLibrary::InvokeGlobalEvent(const UObject* WorldContextObject, UObject* Caller, const FGameplayTag Key, const FInstancedStruct& Payload)
{
	if (URCUtilsSubsystem* Subsystem = URCUtilsSubsystem::Get(WorldContextObject))
		Subsystem->InvokeGlobalEvent(Key, Caller, Payload);
}

void URCGlobalUtilsLibrary::RemoveGlobalEvent(const UObject* WorldContextObject, const FGameplayTag Key)
{
	if (URCUtilsSubsystem* Subsystem = URCUtilsSubsystem::Get(WorldContextObject))
		Subsystem->RemoveGlobalEvent(Key);
}

void URCGlobalUtilsLibrary::RemoveAllGlobalEvents(const UObject* WorldContextObject)
{
	if (URCUtilsSubsystem* Subsystem = URCUtilsSubsystem::Get(WorldContextObject))
		Subsystem->RemoveAllGlobalEvents();
}
