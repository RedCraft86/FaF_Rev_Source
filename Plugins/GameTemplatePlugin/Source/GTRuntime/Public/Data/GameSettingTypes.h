// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameSettingTypes.generated.h"

UENUM(BlueprintType, DisplayName = "Color Blind Mode")
enum class EGTColorBlindMode : uint8
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
enum class EGTAntiAliasingMethod : uint8
{
	// Fast Approximate Anti-Aliasing
	FXAA,

	// Temporal Anti-Aliasing
	TAA,

	// Temporal Super-Resolution
	TSR
};
