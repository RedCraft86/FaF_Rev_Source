// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputSubsystems.h"

AFRPlayerController::AFRPlayerController()
	: MappingContext(nullptr), UnfocusedWidget(nullptr), bLastPaused(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

UEnhancedInputLocalPlayerSubsystem* AFRPlayerController::GetInputSubsystem() const
{
	return GetLocalPlayer() ? GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
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
}
