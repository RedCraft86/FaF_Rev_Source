// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "CoreTypes.generated.h"

UENUM(BlueprintInternalUseOnly)
enum class EGCSearchStateOutputPins : uint8
{
	Found,
	NotFound
};
#define SEARCH_STATE(State) EGCSearchStateOutputPins::State

UENUM(BlueprintInternalUseOnly)
enum class EGCValidStateOutputPins : uint8
{
	Valid,
	Invalid
};
#define VALID_STATE(State) EGCValidStateOutputPins::State