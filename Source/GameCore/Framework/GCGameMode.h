// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "GCGameMode.generated.h"

UCLASS(Abstract, DisplayName = "Main Game Mode")
class GAMECORE_API AGCGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AGCGameMode();

	static AGCGameMode* Get(const UObject* WorldContext);
};
