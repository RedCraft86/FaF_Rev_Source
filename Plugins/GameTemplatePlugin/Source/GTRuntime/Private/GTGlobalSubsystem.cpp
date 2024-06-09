// Copyright (C) RedCraft86. All Rights Reserved.

#include "GTGlobalSubsystem.h"

#include "GTSettings.h"

void UGTGlobalSubsystem::SetGlobalVariable(const FGameplayTag Tag, const FInstancedStruct& Data, const bool bUntilMapChange)
{
	Variables.Add(Tag, Data);
	if (bUntilMapChange) { MapVariables.Add(Tag); } else { MapVariables.Remove(Tag); }
}

FInstancedStruct UGTGlobalSubsystem::GetGlobalVariable(const FGameplayTag Tag) const
{
	return Variables.FindRef(Tag);
}

void UGTGlobalSubsystem::DeleteGlobalVariable(const FGameplayTag Tag)
{
	Variables.Remove(Tag);
	MapVariables.Remove(Tag);
}

void UGTGlobalSubsystem::InvokeGlobalEvent(const FGameplayTag Tag, UObject* Instigator, const FInstancedStruct& Data)
{
	const FGTGlobalEventMulti* Delegate = Events.Find(Tag);
	if (Delegate && Delegate->IsBound())
	{
		Delegate->Broadcast(Instigator, Data);
	}
}

void UGTGlobalSubsystem::BindGlobalEvent(const FGameplayTag Tag, const FGTGlobalEventSingle Callback)
{
	if (Callback.IsBound()) Events.FindOrAdd(Tag).AddUnique(Callback);
}

void UGTGlobalSubsystem::UnbindGlobalEvent(const FGameplayTag Tag, const FGTGlobalEventSingle& Callback)
{
	if (!Callback.IsBound()) return;
	if (FGTGlobalEventMulti* Delegate = Events.Find(Tag))
	{
		Delegate->Remove(Callback);
		if (!Delegate->IsBound())
		{
			DeleteGlobalEvent(Tag);
		}
	}
}

void UGTGlobalSubsystem::DeleteGlobalEvent(const FGameplayTag Tag)
{
	if (FGTGlobalEventMulti* Delegate = Events.Find(Tag))
	{
		Delegate->Clear();
	}

	Events.Remove(Tag);
}

void UGTGlobalSubsystem::PostLoadMap(UWorld* World)
{
	for (const FGameplayTag& Tag : MapVariables)
	{
		Variables.Remove(Tag);
	}
	
	MapVariables.Empty();
}

void UGTGlobalSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Variables = UGTSettings::GetConst()->GlobalVariables;
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UGTGlobalSubsystem::PostLoadMap);
}
