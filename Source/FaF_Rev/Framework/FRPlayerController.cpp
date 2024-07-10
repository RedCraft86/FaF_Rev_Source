// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputSubsystems.h"
#include "GameSection/GameSectionManager.h"
#include "Menus/PauseWidget.h"
#include "FRGameMode.h"

AFRPlayerController::AFRPlayerController()
	: MappingContext(nullptr), UnfocusedWidget(nullptr), PauseWidget(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
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
		if (!PauseWidget)
		{
			PauseWidget = FRGameMode(this)->GetWidget<UPauseWidgetBase>();
		}

		bGamePaused = bInPaused;
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

	if (!FRSettings->IsGameplayMap(this) || UGameSectionManager::Get(this)->IsLoading())
	{
#if !WITH_EDITOR
		UKismetSystemLibrary::ExecuteConsoleCommand(this, FString::Printf(TEXT("t.MaxFPS %f"),
				bFocused ? UGameSettings::Get()->GetFrameRateLimit() : 4.0f));
#endif
		return;
	}
	
#if !WITH_EDITOR
	UKismetSystemLibrary::ExecuteConsoleCommand(this, FString::Printf(TEXT("t.MaxFPS %f"),
		UGameSettings::Get()->GetFrameRateLimit()));
#endif

	if (bGamePaused) return;
	if (bFocused)
	{
		SetPause(false);
		if (UnfocusedWidget->IsInViewport())
		{
			UnfocusedWidget->RemoveFromParent();
		}
	}
	else if (!IsPaused())
	{
		UnfocusedWidget->AddToViewport(100);
		SetPause(true);
	}
}

void AFRPlayerController::BeginPlay()
{
	Super::BeginPlay();
	PlayerCameraManager->SetTickableWhenPaused(true);
	FSlateApplication::Get().OnApplicationActivationStateChanged().AddUObject(this, &AFRPlayerController::OnWindowFocusChanged);
	GetInputSubsystem()->AddMappingContext(MappingContext, 0);
}
