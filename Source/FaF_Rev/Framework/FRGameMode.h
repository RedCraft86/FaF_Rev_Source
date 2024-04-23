// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTGameMode.h"
#include "FRGameMode.generated.h"

#define FRGamemode(Context) AGTGameMode::Get<AFRGameModeBase>(Context)

UCLASS(Abstract)
class FAF_REV_API AFRGameModeBase final : public AGTGameMode
{
	GENERATED_BODY()

public:

	AFRGameModeBase();
};
