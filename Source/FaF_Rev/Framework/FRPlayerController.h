﻿// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTPlayerController.h"
#include "InputMappingContext.h"
#include "FRPlayerController.generated.h"

#define FRPlayerController(Context) AGTPlayerController::Get<AFRPlayerController>(Context)

UCLASS(Abstract)
class FAF_REV_API AFRPlayerController final : public AGTPlayerController
{
	GENERATED_BODY()

public:

	AFRPlayerController();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class UInventoryComponent* Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		TSubclassOf<UUserWidget> UnfocusedWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		UInputMappingContext* MappingContext;

	class UEnhancedInputLocalPlayerSubsystem* GetInputSubsystem() const;
	
	void SetPauseState(bool bInPaused);

protected:
	
	bool bLastPaused;
	UPROPERTY(Transient) UUserWidget* UnfocusedWidget;
	UPROPERTY(Transient) class UPauseWidgetBase* PauseWidget;
	
	void OnWindowFocusChanged(bool bFocused);
	virtual void BeginPlay() override;

};
