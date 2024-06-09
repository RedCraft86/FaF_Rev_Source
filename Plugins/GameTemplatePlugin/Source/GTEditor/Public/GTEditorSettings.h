// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "GTEditorSettings.generated.h"

UCLASS(Config = Editor, DefaultConfig, DisplayName = "Editor Settings")
class GTEDITOR_API UGTEditorSettings final : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	static UGTEditorSettings* Get() { return GetMutableDefault<UGTEditorSettings>(); }
	
	UGTEditorSettings()
	{
		CategoryName = TEXT("GameTemplate");
		SectionName = TEXT("EditorSettings");

		StartupCommands = {
			{ TEXT("r.VSyncEditor"), TEXT("1") },
			{ TEXT("r.Streaming.PoolSize"), TEXT("3000") }
		};
		TodoTaskBoardURL = TEXT("https://trello.com/");
	}

	UPROPERTY(Config, EditAnywhere, Category = "Editor")
		TMap<FString, FString> StartupCommands;

	UPROPERTY(Config, EditAnywhere, Category = "Editor")
		FString TodoTaskBoardURL;
};
