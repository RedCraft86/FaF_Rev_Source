// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GamejoltSettings.generated.h"

UCLASS(Config = Engine, DefaultConfig, DisplayName = "Gamejolt API")
class GAMEJOLTAPI_API UGamejoltSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	
	UGamejoltSettings()
	{
		CategoryName = TEXT("GameTemplate");
		SectionName = TEXT("GamejoltAPI");
		
		GameID = 0;
		GameKey = TEXT("");
	}

	UPROPERTY(Config, EditAnywhere, Category = "GamejoltAPI", meta = (ConfigRestartRequired = true, ClampMin = 0, UIMin = 0))
		int32 GameID;

	UPROPERTY(Config, EditAnywhere, Category = "GamejoltAPI", meta = (ConfigRestartRequired = true))
		FString GameKey;

public: // Statics

	static UGamejoltSettings* Get() { return GetMutableDefault<UGamejoltSettings>(); }
	static const UGamejoltSettings* GetConst() { return GetDefault<UGamejoltSettings>(); }
};
