// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTPlayerController.h"
#include "FRPlayerController.generated.h"

#define FRPlayerController(Context) AGTPlayerController::Get<AFRPlayerController>(Context)

UCLASS(Abstract)
class FAF_REV_API AFRPlayerController final : public AGTPlayerController
{
	GENERATED_BODY()

public:

	AFRPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		TSubclassOf<UUserWidget> UnfocusedWidgetClass;

protected:

	UPROPERTY(Transient, BlueprintReadOnly)
		UUserWidget* UnfocusedWidget;

	bool bLastPaused;
	
	virtual void BeginPlay() override;
	void OnWindowFocusChanged(bool bFocused);
};
