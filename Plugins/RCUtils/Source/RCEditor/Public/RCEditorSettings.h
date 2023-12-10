// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "RCEditorSettings.generated.h"

UCLASS(Config = Editor, DefaultConfig, DisplayName = "Editor Settings")
class RCEDITOR_API URCEditorSettings final : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	URCEditorSettings()
	{
		CategoryName = TEXT("Project");
		SectionName = TEXT("RCEditorSettings");

		StartupConsoleCommands = {
			{ TEXT("r.VSyncEditor"), TEXT("1") },
			{ TEXT("r.Streaming.PoolSize"), TEXT("3000") }
		};
	};

	// Static C++ getter to get the Developer Settings object of this game editor
	static URCEditorSettings* Get()
	{
		URCEditorSettings* Settings = GetMutableDefault<URCEditorSettings>();
		check(Settings);

		return Settings;
	};

	UPROPERTY(Config, EditAnywhere, Category = "EditorSettings")
		TMap<FString, FString> StartupConsoleCommands;
};
