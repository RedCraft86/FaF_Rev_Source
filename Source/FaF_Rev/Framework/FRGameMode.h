// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTGameMode.h"
#include "FRGameMode.generated.h"

#define GET_GAMEMODE(Context) AGTGameMode::Get<AFRGameMode>(Context)

UCLASS()
class FAF_REV_API AFRGameMode final : public AGTGameMode
{
	GENERATED_BODY()

	AFRGameMode();
};
