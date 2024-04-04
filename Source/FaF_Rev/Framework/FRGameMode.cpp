// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRGameMode.h"
#include "FRController.h"
#include "FRPlayer.h"

AFRGameMode::AFRGameMode()
{
	PlayerControllerClass = AFRController::StaticClass();
	DefaultPawnClass = AFRPlayer::StaticClass();
}
