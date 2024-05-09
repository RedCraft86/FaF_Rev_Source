// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRGameMode.h"
#include "Menus/PhotoModeWidget.h"
#include "FRPlayerController.h"
#include "FRGameState.h"
#include "FRPlayer.h"

AFRGameModeBase::AFRGameModeBase()
	: PhotoModeActor(nullptr), InspectionActor(nullptr)
{
	PlayerControllerClass = AFRPlayerController::StaticClass();
	DefaultPawnClass = AFRPlayerBase::StaticClass();
	GameStateClass = AFRGameStateBase::StaticClass();
}

void AFRGameModeBase::PlayerInitialized()
{
	if (UPhotoModeWidgetBase* PhotoModeWidget = GetWidget<UPhotoModeWidgetBase>())
	{
		PhotoModeWidget->SetPhotoModeActor(PhotoModeActor);
		PhotoModeActor->PhotoModeWidget = PhotoModeWidget;
	}
}
