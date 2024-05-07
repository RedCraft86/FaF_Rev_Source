// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "EnemyData.generated.h"

UENUM(BlueprintType, DisplayName = "Enemy AI Mode")
enum class EEnemyAIMode : uint8
{
	None,
	Suspicion,
	Chase,
	Search
};
ENUM_RANGE_BY_FIRST_AND_LAST(EEnemyAIMode, EEnemyAIMode::None, EEnemyAIMode::Search);