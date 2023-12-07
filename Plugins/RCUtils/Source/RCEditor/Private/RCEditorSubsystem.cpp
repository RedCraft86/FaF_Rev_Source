// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "RCEditorSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "RCEditorSettings.h"

void URCEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	URCEditorSettings* Settings = URCEditorSettings::Get();
	for (const TPair<FString, FString>& Pair : Settings->StartupConsoleCommands)
	{
		UKismetSystemLibrary::ExecuteConsoleCommand(this,
			FString::Printf(TEXT("%s %s"),
				*Pair.Key.Replace(TEXT(" "), TEXT("")),
				*Pair.Value));
	}
}
