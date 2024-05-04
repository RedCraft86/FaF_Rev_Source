// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRPlayerController.h"
#include "Blueprint/UserWidget.h"

AFRPlayerController::AFRPlayerController() : UnfocusedWidget(nullptr), bLastPaused(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFRPlayerController::BeginPlay()
{
	Super::BeginPlay();
	FSlateApplication::Get().OnApplicationActivationStateChanged().AddUObject(this, &AFRPlayerController::OnWindowFocusChanged);
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
