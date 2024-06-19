// Copyright (C) RedCraft86. All Rights Reserved.

#include "InventoryComponent.h"
#include "Menus/InventoryWidget.h"
#include "FRPlayerController.h"
#include "ConsumableActor.h"
#include "FRGameMode.h"
#include "FRPlayer.h"

UInventoryComponent::UInventoryComponent() : PlayerChar(nullptr), InventoryPreview(nullptr), InventoryWidget(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UInventoryComponent::OpenUI()
{
	if (!InventoryWidget) return;
	if (InventoryWidget->IsInViewport()) return;
	PlayerChar->GetPlayerController()->SetPause(true);
	PlayerChar->AddLockFlag(Player::LockFlags::Inventory);
	PlayerChar->GetGameMode()->SetGameInputMode(EGameInputMode::GameAndUI, true,
		EMouseLockMode::LockAlways, false, InventoryWidget);

	CleanInventory();
	InventoryWidget->AddWidget();
	if (InventoryPreview)
	{
		InventoryPreview->Initialize();
		InventoryPreview->SetItem({});
	}
}

void UInventoryComponent::CloseUI() const
{
	if (!InventoryWidget) return;
	if (!InventoryWidget->IsInViewport()) return;
	PlayerChar->GetPlayerController()->SetPause(false);
	PlayerChar->ClearLockFlag(Player::LockFlags::Inventory);
	PlayerChar->GetGameMode()->SetGameInputMode(EGameInputMode::GameOnly);
	
	InventoryWidget->RemoveWidget();
	if (InventoryPreview)
	{
		InventoryPreview->Deinitialize();
	}
}

void UInventoryComponent::EquipmentUse() const
{
	if (EquipmentData.Equipment)
	{
		EquipmentData.Equipment->OnUse();
	}
}

void UInventoryComponent::EquipmentUseAlt(const bool bPressed) const
{
	if (!EquipmentData.Equipment) return;
	if (bPressed)
	{
		EquipmentData.Equipment->OnStartAltUse();
	}
	else
	{
		EquipmentData.Equipment->OnEndAltUse();
	}
}

void UInventoryComponent::SetInventoryPreview(AInventoryPreview* InActor)
{
	InventoryPreview = InActor;
	if(InventoryPreview) InventoryPreview->Inventory = this;
}

TArray<FGuid> UInventoryComponent::GetSortedSlots(const EInventoryItemType TypeFilter)
{
	CleanInventory();
	TArray<FGuid> Result;
	ItemSlots.GenerateKeyArray(Result);
	Result.Sort([this](const FGuid& A, const FGuid& B)->bool
	{
		const UInventoryItemData* ItemA = ItemSlots[A].GetItemData<UInventoryItemData>();
		const UInventoryItemData* ItemB = ItemSlots[B].GetItemData<UInventoryItemData>();
		return ItemA->DisplayName.ToString() < ItemB->DisplayName.ToString() && ItemA->Priority <= ItemB->Priority;
	});

	if (TypeFilter != EInventoryItemType::Any)
	{
		Result.RemoveAll([this, TypeFilter](const FGuid& Elem)
		{
			return ItemSlots[Elem].GetItemData<UInventoryItemData>()->ItemType == TypeFilter;
		});
	}

	return Result;
}

bool UInventoryComponent::UseKeyItem(const UInventoryItemDataBase* InItem, const FString& KeyID)
{
	if (!InItem || KeyID.IsEmpty())
		return false;
	
	FInventoryItemFilter Filter;
	Filter.MetaFilter = EInventoryMetaFilter::MatchAll;
	Filter.Metadata.Add(NativeItemKeys::KeyID, KeyID);
	const FGuid Slot = FindSlot(InItem, Filter);
	if (!Slot.IsValid()) return false;
	
	if (SlotHasMetadata(Slot, NativeItemKeys::SingleKey))
		RemoveItemFromSlot(Slot, 1);
	
	return true;
}

void UInventoryComponent::UnequipItem()
{
	if (!EquipmentData.ItemID.IsValid()) return;
}

void UInventoryComponent::EquipItem(const FGuid& ItemKey)
{
	if (EquipmentData.ItemID.IsValid()) return;
}

void UInventoryComponent::ConsumeItem(const FGuid& ItemKey)
{
	const UInventoryItemData* ItemData = ItemSlots.FindRef(ItemKey).GetItemData<UInventoryItemData>();
	if (ItemData && ItemData->ItemType == EInventoryItemType::Consumable && ItemData->ConsumableClass)
	{
		AConsumableActor* Consumable = GetWorld()->SpawnActor<AConsumableActor>(ItemData->ConsumableClass);
		ItemSlots[ItemKey].Amount -= Consumable->UseItem() ? 1 : 0;
		if (ItemSlots[ItemKey].Amount <= 0)
		{
			ItemSlots.Remove(ItemKey);
		}
	}
}

void UInventoryComponent::ImportSaveData(const FInventorySaveData& InData)
{
	CurrencyData = InData.CurrencyData;
	if (ItemSlots.Contains(InData.ActiveEquipment))
	{
		// EQUIPMENT
	}

	ON_UPDATE();
}

FInventorySaveData UInventoryComponent::ExportSaveData()
{
	return {EquipmentData.ItemID, CurrencyData};
}
