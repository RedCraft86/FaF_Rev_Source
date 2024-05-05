// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRGameMode.h"
#include "FRPlayerController.h"
#include "FRPlayer.h"

AFRGameModeBase::AFRGameModeBase()
	: PhotoModeActor(nullptr), InspectionActor(nullptr)
{
	PlayerControllerClass = AFRPlayerController::StaticClass();
	DefaultPawnClass = AFRPlayerBase::StaticClass();
}
