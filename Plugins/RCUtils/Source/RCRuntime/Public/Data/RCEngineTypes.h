// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once 

#include "Curves/CurveLinearColor.h"
#include "RCEngineTypes.generated.h"

UENUM(BlueprintType)
enum class ERCArchiveCompressionFormat : uint8
{
	Zlib,
	Gzip
};

UENUM(BlueprintType)
enum class ERCOodleCompressor : uint8
{
	Selkie,
	Mermaid,
	Kraken,
	Leviathan
};

UENUM(BlueprintType)
enum class ERCOodleLevel : uint8
{
	HyperFast4,
	HyperFast3,
	HyperFast2,
	HyperFast1,
	SuperFast,
	VeryFast,
	Fast,
	Normal,
	Optimal1,
	Optimal2,
	Optimal3,
	Optimal4,
};

UENUM(BlueprintType, DisplayName = "Key Trigger Type")
enum class ERCKeyTriggerType : uint8
{
	Pressed,
	Released,
	Clicked
};

USTRUCT(BlueprintType)
struct RCRUNTIME_API FRuntimeLinearColorCurve
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "RuntimeLinearColorCurve")
		FRuntimeCurveLinearColor Curve;

	const FRichCurve* GetRichCurveConst(const int32 Index) const
	{
		if (Index < 0 || Index >= 4)
			return nullptr;

		if (Curve.ExternalCurve != nullptr)
		{
			return &(Curve.ExternalCurve->FloatCurves[Index]);
		}
        
		return &(Curve.ColorCurves[Index]);
	}
};
