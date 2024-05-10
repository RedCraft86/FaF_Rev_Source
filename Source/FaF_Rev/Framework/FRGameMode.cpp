// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRGameMode.h"
#include "NarrativeComponent.h"
#include "Inventory/InventoryComponent.h"
#include "FRPlayerController.h"
#include "FRGameState.h"
#include "FRPlayer.h"
#include "Core/NarrativeWidget.h"

AFRGameModeBase::AFRGameModeBase()
	: InspectionActor(nullptr)
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
}
