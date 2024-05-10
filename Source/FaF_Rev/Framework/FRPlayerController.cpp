// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputSubsystems.h"
#include "Inventory/InventoryComponent.h"
#include "NarrativeComponent.h"
#include "Menus/PauseWidget.h"
#include "FRGameMode.h"

AFRPlayerController::AFRPlayerController()
	: MappingContext(nullptr), bLastPaused(false), UnfocusedWidget(nullptr), PauseWidget(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	Inventory = CreateDefaultSubobject<UInventoryComponent>("Inventory");
	Narrative = CreateDefaultSubobject<UNarrativeComponent>("Narrative");
}

UEnhancedInputLocalPlayerSubsystem* AFRPlayerController::GetInputSubsystem() const
{
	return GetLocalPlayer() ? GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
}

void AFRPlayerController::SetPauseState(const bool bInPaused)
{
	if (IsPaused() != bInPaused)
	{
		SetPause(bInPaused);
		if (bInPaused)
		{
			PauseWidget->AddWidget(nullptr);
		}
		else
		{
			PauseWidget->RemoveWidget(nullptr);
		}
	}
}

void AFRPlayerController::OnWindowFocusChanged(bool bFocused)
{
	if (!IsValid(UnfocusedWidget))
	{
		UnfocusedWidget = CreateWidget<UUserWidget>(this, UnfocusedWidgetClass);
	}
	
	if (bFocused)
	{
		UnfocusedWidget->RemoveFromParent();
		SetPause(bLastPaused);
	}
	else
	{
		UnfocusedWidget->AddToViewport(100);
		bLastPaused = IsPaused();
		SetPause(true);
	}
}

void AFRPlayerController::BeginPlay()
{
	Super::BeginPlay();
	PlayerCameraManager->SetTickableWhenPaused(true);
	FSlateApplication::Get().OnApplicationActivationStateChanged().AddUObject(this, &AFRPlayerController::OnWindowFocusChanged);

	PauseWidget = FRGameMode(this)->GetWidget<UPauseWidgetBase>();
}
