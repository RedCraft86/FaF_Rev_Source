// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRGameMode.h"
#include "FRController.h"
#include "GTUserWidget.h"
#include "FRPlayer.h"

AFRGameModeBase::AFRGameModeBase()
{
	PlayerControllerClass = AFRControllerBase::StaticClass();
	DefaultPawnClass = AFRPlayerBase::StaticClass();
}
