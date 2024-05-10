﻿// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRGameMode.h"
#include "NarrativeComponent.h"
#include "Inventory/InventoryComponent.h"
#include "FRPlayerController.h"
#include "FRGameState.h"
#include "FRPlayer.h"

AFRGameModeBase::AFRGameModeBase()
	: InspectionActor(nullptr), PlayerController(nullptr), PlayerCharacter(nullptr)
{
	PlayerControllerClass = AFRPlayerController::StaticClass();
	DefaultPawnClass = AFRPlayerBase::StaticClass();
	GameStateClass = AFRGameStateBase::StaticClass();
	
	Narrative = CreateDefaultSubobject<UNarrativeComponent>("Narrative");
	Inventory = CreateDefaultSubobject<UInventoryComponent>("Inventory");
}
