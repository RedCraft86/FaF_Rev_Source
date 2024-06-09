// Copyright (C) RedCraft86. All Rights Reserved.

#include "GTEditorSubsystem.h"
#include "GTEditorSettings.h"

void UGTEditorSubsystem::RunCommand(const FString& Command) const
{
	UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::ReturnNull);
	if (!IConsoleManager::Get().ProcessUserConsoleInput(*Command, *GLog, World))
	{
		GEngine->Exec(World, *Command);
	}
}

void UGTEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	const TMap<FString, FString> Commands = UGTEditorSettings::Get()->StartupCommands;
	for (const TPair<FString, FString>& Command : Commands)
	{
		RunCommand(FString::Printf(TEXT("%s %s"), *Command.Key, *Command.Value));
	}
}
