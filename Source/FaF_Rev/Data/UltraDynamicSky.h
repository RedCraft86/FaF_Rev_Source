// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "UltraDynamicSky.generated.h"

UCLASS(Abstract, Blueprintable)
class UWeatherDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
};

UENUM(BlueprintType, DisplayName = "UDS Cloud Type")
enum class EUDSCloudType : uint8
{
	Volumetric,
	Dynamic2D,
	None,
};

USTRUCT(BlueprintType, DisplayName = "UDS Settings")
struct FAF_REV_API FUDSSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		EUDSCloudType CloudType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		float NightBrightness;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		float FogBrightness;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		float SkyLightIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Weather")
		float WeatherVolume;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Weather")
		TSoftObjectPtr<UWeatherDataAsset> WeatherPreset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Weather")
		float WeatherFadeTime;

	FUDSSettings()
		: CloudType(EUDSCloudType::Volumetric)
		, NightBrightness(2.5f)
		, FogBrightness(1.0f)
		, SkyLightIntensity(4.0f)
		, WeatherVolume(1.0f)
		, WeatherPreset(nullptr)
		, WeatherFadeTime(1.0f)
	{}
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
