// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "DifficultyData.generated.h"

UENUM(BlueprintType)
enum class EDifficultyMode : uint8
{
	Easy,
	Normal,
	Hard
};
ENUM_RANGE_BY_FIRST_AND_LAST(EDifficultyMode, EDifficultyMode::Easy, EDifficultyMode::Hard);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDifficultyChangedBP, const EDifficultyMode, Mode);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDifficultyChanged, const EDifficultyMode);
