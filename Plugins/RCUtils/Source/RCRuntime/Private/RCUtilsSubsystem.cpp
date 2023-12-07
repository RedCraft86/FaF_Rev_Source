// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "RCUtilsSubsystem.h"
#include "RCRuntimeSettings.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#include "GeneralProjectSettings.h"

URCUtilsSubsystem::URCUtilsSubsystem()
{
	LevelVariables = {};
	GlobalVariables = {};
	GlobalEvents = {};

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &URCUtilsSubsystem::PostLoadMap);
}

URCUtilsSubsystem* URCUtilsSubsystem::Get(const UObject* WorldContext)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	const UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
	return GI ? GI->GetSubsystem<URCUtilsSubsystem>() : nullptr;
}

void URCUtilsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const URCRuntimeSettings* Settings = URCRuntimeSettings::Get();
	
#if !WITH_EDITOR // Only one instance of the game can be initialized in non-editor builds
	if (Settings->bAllowSingleInstanceOnly)
	{
		const UGeneralProjectSettings& ProjectSettings = *GetDefault<UGeneralProjectSettings>();
		const FString LockFilePath = FPlatformProcess::UserTempDir() + ProjectSettings.ProjectID.ToString();
		if (!IFileManager::Get().CreateFileWriter(*LockFilePath, 0))
		{
			FPlatformApplicationMisc::RequestMinimize();
			FPlatformMisc::RequestExit(0);
			return;
		}
	}
#endif
	
	for (const TPair<FString, FString>& Command : Settings->StartupCommands)
    {
		UKismetSystemLibrary::ExecuteConsoleCommand(GetGameInstance(),
			FString::Printf(TEXT("%s%s"),
				*Command.Key.Replace(TEXT(" "), TEXT("")),
				*(Command.Value.IsEmpty() ? TEXT("") : TEXT(" ") + Command.Value)));
    }
    
	GlobalVariables = Settings->StartupVariables;
}

void URCUtilsSubsystem::SetGlobalVariable(const FGameplayTag& InKey, const FInstancedStruct& InStruct, const bool bLevelVariable)
{
	if (!InKey.IsValid())
		return;

	if (InStruct.IsValid())
	{
		GlobalVariables.Add(InKey, InStruct);
		if (bLevelVariable)
		{
			LevelVariables.Add(InKey);
		}
		else
		{
			LevelVariables.Remove(InKey);
		}
	}
	else
	{
		GlobalVariables.Remove(InKey);
		LevelVariables.Remove(InKey);
	}
}

FInstancedStruct URCUtilsSubsystem::GetGlobalVariable(const FGameplayTag& InKey) const
{
	if (!InKey.IsValid())
	{
		return {};
	}

	return GlobalVariables.FindRef(InKey);
}

void URCUtilsSubsystem::RemoveGlobalVariable(const FGameplayTag& InKey)
{
	if (InKey.IsValid())
	{
		GlobalVariables.Remove(InKey);
	}
}

void URCUtilsSubsystem::RemoveAllGlobalVariables()
{
	GlobalVariables.Empty();
}

void URCUtilsSubsystem::BindToGlobalEvent(const FGameplayTag& InKey, const FRCGlobalEventSingle& InDelegate)
{
	if (InKey.IsValid() && InDelegate.IsBound())
	{
		GlobalEvents.FindOrAdd(InKey).AddUnique(InDelegate);
	}
}

void URCUtilsSubsystem::UnbindFromGlobalEvent(const FGameplayTag& InKey, const FRCGlobalEventSingle& InDelegate)
{
	if (!InKey.IsValid() || !InDelegate.IsBound())
	{
		return;
	}

	if (GlobalEvents.Contains(InKey))
	{
		GlobalEvents.FindRef(InKey).Remove(InDelegate);
		if (!GlobalEvents.FindRef(InKey).IsBound())
		{
			RemoveGlobalEvent(InKey);
		}
	}
}

void URCUtilsSubsystem::InvokeGlobalEvent(const FGameplayTag& InKey, UObject* Caller, const FInstancedStruct& InPayload) const
{
	if (InKey.IsValid() && GlobalEvents.Contains(InKey))
	{
		GlobalEvents.FindRef(InKey).Broadcast(Caller, InPayload);
	}
}

void URCUtilsSubsystem::RemoveGlobalEvent(const FGameplayTag& InKey)
{
	if (InKey.IsValid() && GlobalEvents.Contains(InKey))
	{
		GlobalEvents.FindRef(InKey).Clear();
		GlobalEvents.Remove(InKey);
	}
}

void URCUtilsSubsystem::RemoveAllGlobalEvents()
{
	TArray<FGameplayTag> Keys;
	GlobalEvents.GenerateKeyArray(Keys);
	for (const FGameplayTag& Key : Keys)
	{
		RemoveGlobalEvent(Key);
	}
}

void URCUtilsSubsystem::PostLoadMap(UWorld* World)
{
	for (const FGameplayTag& VarKey : LevelVariables)
	{
		if (VarKey.IsValid())
		{
			GlobalVariables.Remove(VarKey);
		}
	}
	LevelVariables.Empty();

	TArray<FGameplayTag> EventKeys;
	GlobalEvents.GenerateKeyArray(EventKeys);
	for (const FGameplayTag& EventKey : EventKeys)
	{
		if (GlobalEvents.Contains(EventKey) && !GlobalEvents.FindRef(EventKey).IsBound())
		{
			GlobalEvents.Remove(EventKey);
		}
	}
}
