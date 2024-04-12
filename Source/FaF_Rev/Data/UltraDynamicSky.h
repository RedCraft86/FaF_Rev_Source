// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "UltraDynamicSky.generated.h"

USTRUCT(BlueprintType, DisplayName = "UDS Settings")
struct FAF_REV_API FUDSSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Settings")
	float sample;

	FUDSSettings() : sample(0.0f) {}
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
};
