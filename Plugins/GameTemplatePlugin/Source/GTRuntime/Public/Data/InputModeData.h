// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "InputModeData.generated.h"

UENUM(BlueprintType)
enum class EGameInputMode : uint8
{
	UI_Only,
	GameOnly,
	GameAndUI
};

USTRUCT(BlueprintType)
struct GTRUNTIME_API FGameInputModeData
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category = "InputModeData")
		EGameInputMode InputMode;

	UPROPERTY(EditAnywhere, Category = "InputModeData", meta = (EditCondition = "InputMode != EGameInputMode::GameOnly", EditConditionHides))
		bool bShowMouseCursor;

	UPROPERTY(EditAnywhere, Category = "InputModeData", meta = (EditCondition = "InputMode != EGameInputMode::GameOnly", EditConditionHides))
		EMouseLockMode MouseLock;
	
	UPROPERTY(EditAnywhere, Category = "InputModeData", meta = (EditCondition = "InputMode == EGameInputMode::GameAndUI", EditConditionHides))
		bool bHideCursorOnCapture;

	UPROPERTY(EditAnywhere, Category = "InputModeData", meta = (EditCondition = false, DisplayThumbnail = false))
		TObjectPtr<UUserWidget> FocusWidget;

	FInputModeUIOnly UI_Only;
	FInputModeGameOnly GameOnly;
	FInputModeGameAndUI GameAndUI;

public:
	
	FGameInputModeData()
		: InputMode(EGameInputMode::GameOnly)
		, bShowMouseCursor(false)
		, MouseLock(EMouseLockMode::LockAlways)
		, bHideCursorOnCapture(true)
		, FocusWidget(nullptr)
	{}

	FGameInputModeData(const EGameInputMode InInputMode, const bool bInShowMouseCursor, const EMouseLockMode InMouseLock = EMouseLockMode::LockAlways, const bool bInHideCursorOnCapture = true, UUserWidget* InFocusWidget = nullptr)
		: InputMode(InInputMode)
		, bShowMouseCursor(bInShowMouseCursor)
		, MouseLock(InMouseLock)
		, bHideCursorOnCapture(bInHideCursorOnCapture)
		, FocusWidget(InFocusWidget)
	{}

	void ClearReferences()
	{
		UI_Only = FInputModeUIOnly();
		GameAndUI = FInputModeGameAndUI();
		FocusWidget = nullptr;
	}

	bool ShouldShowMouse() const { return InputMode != EGameInputMode::GameOnly && bShowMouseCursor; }
	
	const FInputModeDataBase& GetInputMode()
	{
		switch (InputMode)
		{
		case EGameInputMode::UI_Only:
			UI_Only.SetLockMouseToViewportBehavior(MouseLock);
			if (FocusWidget) UI_Only.SetWidgetToFocus(FocusWidget->TakeWidget());
			return UI_Only;
		
		case EGameInputMode::GameAndUI:
			GameAndUI.SetLockMouseToViewportBehavior(MouseLock);
			GameAndUI.SetHideCursorDuringCapture(bHideCursorOnCapture);
			if (FocusWidget) GameAndUI.SetWidgetToFocus(FocusWidget->TakeWidget());
			return GameAndUI;

		default: return GameOnly;
		}
	}
};
