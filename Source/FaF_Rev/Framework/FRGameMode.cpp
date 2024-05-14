// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRGameMode.h"
#include "NarrativeComponent.h"
#include "Inventory/InventoryComponent.h"
#include "UserWidgets/Menus/InventoryWidget.h"
#include "FRPlayerController.h"
#include "FRGameState.h"
#include "FRPlayer.h"

AFRGameModeBase::AFRGameModeBase()
	: PlayerController(nullptr), PlayerCharacter(nullptr)
{
	PlayerControllerClass = AFRPlayerController::StaticClass();
	DefaultPawnClass = AFRPlayerBase::StaticClass();
	GameStateClass = AFRGameStateBase::StaticClass();
	
	Narrative = CreateDefaultSubobject<UNarrativeComponent>("Narrative");
	Inventory = CreateDefaultSubobject<UInventoryComponent>("Inventory");
}

void AFRGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	Inventory->InventoryWidget = GetWidget<UInventoryWidgetBase>();
}
