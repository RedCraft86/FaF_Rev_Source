// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "InstancedStruct.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "Styles/ExpressiveTextStyleBase.h"
#include "GTSettings.generated.h"

UCLASS(Config = Game, DefaultConfig, DisplayName = "Runtime Settings")
class GTRUNTIME_API UGTSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UGTSettings()
	{
		CategoryName = TEXT("GameTemplate");
		SectionName = TEXT("RuntimeSettings");
	}

	UPROPERTY(Config, EditAnywhere, Category = "Settings")
		TSoftObjectPtr<class UMusicDataBase> WorldMusic;
	
	UPROPERTY(Config, EditAnywhere, Category = "Settings")
		TSoftObjectPtr<UExpressiveTextStyleBase> DefaultTextStyle;

	UPROPERTY(Config, EditAnywhere, Category = "Settings")
		TMap<FGameplayTag, FInstancedStruct> GlobalVariables;
	
public: // Statics
	
	static UGTSettings* Get() { return GetMutableDefault<UGTSettings>(); }
	static const UGTSettings* GetConst() { return GetDefault<UGTSettings>(); }
};
