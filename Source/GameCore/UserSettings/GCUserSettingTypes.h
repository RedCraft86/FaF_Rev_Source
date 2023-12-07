// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "Core/CoreMacros.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "GCUserSettingTypes.generated.h"

UENUM(BlueprintType, DisplayName = "Color Blind Mode")
enum class EGCColorBlindMode : uint8
{
	None = 0,
	
	// Deuteranope (red-green weak/blind)
	Deuteranope = 1,
	
	// Protanope (red weak/blind)
	Protanope = 2,
	
	// Tritanope (blue-yellow weak / bind)
	Tritanope = 3
};

UENUM(BlueprintType, DisplayName = "Anti Aliasing Method")
enum class EGCAntiAliasingMethod : uint8
{
	FXAA = 0,
	TAA = 1,
	TSR = 2
};

UENUM(BlueprintType, DisplayName = "Sound Type")
enum class EGCSoundType : uint8
{
	Master,
	Ambience,
	Music,
	SoundFX,
	Voice,

	Max UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EGCSoundType, EGCSoundType::Max);

USTRUCT(BlueprintType, DisplayName = "Player Mappable Key Name")
struct GAMECORE_API FGCPlayerMappableKeyName : public FPulldownStructBase
{
	GENERATED_BODY()
	SETUP_PULLDOWN(FGCPlayerMappableKeyName);
};