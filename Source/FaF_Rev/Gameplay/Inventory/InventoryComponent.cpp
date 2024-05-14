// Copyright (C) RedCraft86. All Rights Reserved.

#include "InventoryComponent.h"
#include "Menus/InventoryWidget.h"
#include "FRPlayerController.h"
#include "FRGameMode.h"
#include "FRPlayer.h"

UInventoryComponent::UInventoryComponent() : PlayerChar(nullptr), InspectionActor(nullptr), InventoryWidget(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UInventoryComponent::OpenUI() const
{
	if (InventoryWidget->IsInViewport()) return;
	PlayerChar->AddLockFlag(Player::LockFlags::Inventory);
	PlayerChar->GetPlayerController()->SetPause(true);
	PlayerChar->GetGameMode()->SetGameInputMode(EGameInputMode::GameAndUI, true,
		EMouseLockMode::LockAlways, false, InventoryWidget);
	
	InventoryWidget->AddWidget();
	if (InspectionActor)
	{
		InspectionActor->Initialize();
		InspectionActor->SetItem({});
	}
}

void UInventoryComponent::CloseUI() const
{
	if (!InventoryWidget->IsInViewport()) return;
	PlayerChar->AddLockFlag(Player::LockFlags::Inventory);
	PlayerChar->GetGameMode()->SetGameInputMode(EGameInputMode::GameOnly);
	PlayerChar->GetPlayerController()->SetPause(false);
	
	InventoryWidget->RemoveWidget();
	if (InspectionActor)
	{
		InspectionActor->Deinitialize();
	}
}

void UInventoryComponent::EquipmentUse()
{
}

void UInventoryComponent::EquipmentUseAlt(bool bPressed)
{
}

void UInventoryComponent::SetInspectionActor(AInspectionActor* InActor)
{
	InspectionActor = InActor;
	InspectionActor->Inventory = this;
}

void UInventoryComponent::LoadSaveData(const FInventorySaveData& InData)
{
}

FInventorySaveData UInventoryComponent::ExportSaveData()
{
	return {{}, CurrencyData, ItemSlots};
}
