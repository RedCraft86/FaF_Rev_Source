// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameSettingTypes.generated.h"

UENUM(BlueprintType, DisplayName = "Color Blind Mode")
enum class EFRColorBlind : uint8
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
enum class EFRAntiAliasing : uint8
{
	// Fast Approximate Anti-Aliasing
	FXAA,

	// Temporal Anti-Aliasing
	TAA,

	// Temporal Super-Resolution
	TSR
};

UENUM(BlueprintType, DisplayName = "Sound Type")
enum class EFRSoundType : uint8
{
	Master,
	Ambience,
	Music,
	SoundFX,
	Voice
};
ENUM_RANGE_BY_FIRST_AND_LAST(EFRSoundType, EFRSoundType::Master, EFRSoundType::Voice);