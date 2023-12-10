// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "InstancedStruct.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "RCRuntimeSettings.generated.h"

UCLASS(Config = Game, DefaultConfig, DisplayName = "RCRuntime")
class RCRUNTIME_API URCRuntimeSettings final : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	URCRuntimeSettings()
	{
		CategoryName = TEXT("Game");
		SectionName = TEXT("RCRuntimeSettings");

		bAllowSingleInstanceOnly = true;
		StartupCommands = {};
		StartupVariables = {};
	}

	static URCRuntimeSettings* Get()
	{
		URCRuntimeSettings* Settings = GetMutableDefault<URCRuntimeSettings>();
		check(Settings);
		return Settings;
	}
	
	UPROPERTY(Config, EditAnywhere, Category = "Startup")
		bool bAllowSingleInstanceOnly;
	
	UPROPERTY(Config, EditAnywhere, Category = "Startup")
		TMap<FString, FString> StartupCommands;

	UPROPERTY(Config, EditAnywhere, Category = "Startup", meta = (ForceInlineRow))
		TMap<FGameplayTag, FInstancedStruct> StartupVariables;
};
