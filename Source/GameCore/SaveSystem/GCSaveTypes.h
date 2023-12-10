// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "GCSaveTypes.generated.h"

UENUM(BlueprintType, DisplayName = "Save Game Error")
enum class EGCSaveGameError : uint8
{
	None,
	Compress,
	Decompress,
	FileWrite,
	FileRead,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGCSaveLoadCompleteSignature, EGCSaveGameError, Error);