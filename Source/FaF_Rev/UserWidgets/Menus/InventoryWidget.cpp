// Copyright (C) RedCraft86. All Rights Reserved.

#include "InventoryWidget.h"
#include "FRGameMode.h"

UInventoryWidgetBase::UInventoryWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer)
{
	ZOrder = 97;
	bAutoAdd = false;
}

void UInventoryWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	GetGameMode<AFRGameModeBase>()->SetGameInputMode(EGameInputMode::GameAndUI, true,
		EMouseLockMode::LockAlways, false, this);
}

void UInventoryWidgetBase::NativeDestruct()
{
	Super::NativeDestruct();
	GetGameMode<AFRGameModeBase>()->SetGameInputMode(EGameInputMode::GameOnly);
}
