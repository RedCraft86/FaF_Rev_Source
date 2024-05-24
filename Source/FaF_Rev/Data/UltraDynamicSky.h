// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "UltraDynamicSky.generated.h"

UCLASS(Abstract, Blueprintable)
class UWeatherDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType, DisplayName = "UDS Settings")
struct FAF_REV_API FUDSSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		TSoftObjectPtr<UWeatherDataAsset> WeatherPreset;

	FUDSSettings() : WeatherPreset(nullptr) {}
};

UINTERFACE(DisplayName = "UDS Interface")
class UUDSInterface : public UInterface
{
	GENERATED_BODY()
};

class FAF_REV_API IUDSInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "UDS", meta = (ForceAsFunction = true))
		void SetCustomSettings(const FUDSSettings& InSettings);

	static void SetCustomSettings(UObject* Target, const FUDSSettings& InSettings)
	{
		if (IsValid(Target) && Target->Implements<UUDSInterface>())
		{
			IUDSInterface::Execute_SetCustomSettings(Target, InSettings);
		}
	}
};
