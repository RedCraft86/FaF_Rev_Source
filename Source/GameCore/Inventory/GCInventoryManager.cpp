// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "GCInventoryManager.h"
#include "GCInventoryConsumable.h"
#include "Player/GCPlayerController.h"
#include "SaveSystem/GCSaveObjects.h"
#include "Core/GCSettings.h"

UGCInventoryManager::UGCInventoryManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	ActiveEquipment = {};
	ConsumableObjects = {};

	Currency = {0};
	ItemSlots = {};
}

UGCInventoryManager* UGCInventoryManager::Get(const UObject* WorldContext)
{
	const AGCPlayerController* PC = AGCPlayerController::Get(WorldContext);
	return PC ? PC->GetInventoryManager() : nullptr;
}

TMap<FName, FGCInvItemSlotData> UGCInventoryManager::GetItemSlotsByType(const EGCInvItemType InType) const
{
	TMap<FName, FGCInvItemSlotData> Result = ItemSlots;
	Result.FilterByPredicate([InType](const TPair<FName, FGCInvItemSlotData>& Pair)->bool{
		return Pair.Value.Type == InType;
	});
	
	return Result;
}

TArray<FName> UGCInventoryManager::GetSortedItemIDs() const
{
	TArray<FName> Result;
	ItemSlots.GenerateKeyArray(Result);
	Result.Sort([](const FName& A, const FName& B)->bool{
		return GetItemData(A).DisplayName.ToString() < GetItemData(B).DisplayName.ToString() &&
			GetItemData(A).Priority <= GetItemData(B).Priority;
	});
	return Result;
}

TArray<FName> UGCInventoryManager::GetSortedItemIDsByType(const EGCInvItemType InType) const
{
	const TMap<FName, FGCInvItemSlotData> FilteredSlots = GetItemSlotsByType(InType);
	
	TArray<FName> Result;
	FilteredSlots.GenerateKeyArray(Result);
	Result.Sort([](const FName& A, const FName& B)->bool{
		return GetItemData(A).DisplayName.ToString() < GetItemData(B).DisplayName.ToString() &&
			GetItemData(A).Priority <= GetItemData(B).Priority;
	});
	return Result;
}

bool UGCInventoryManager::HaveItemSlot(const FGCInvItemID InItemID) const
{
	return ItemSlots.Contains(*InItemID) ? ItemSlots.FindRef(*InItemID).Amount != 0 : false;
}

FGCInvItemSlotData UGCInventoryManager::GetItemSlot(const FGCInvItemID InItemID) const
{
	return ItemSlots.FindRef(*InItemID);
}

EGCInvErrorType UGCInventoryManager::AddItemToInventory(const FGCInvItemID InItemID, const uint8 InAmount, const TArray<uint8>& InBinary, const bool bAutoEquip, const bool bAddUntilOverflow, const bool bSilent, int32& OverflowAmount)
{
	if (InAmount == 0 || InItemID == UGCSettings::Get()->InventoryDefaultItemID)
	{
		OverflowAmount = 0;
		return EGCInvErrorType::None;
	}

	const FGCInvItemData ItemData = GetItemData(InItemID);
	if (!ItemData.IsValidData())
	{
		OverflowAmount = InAmount;
		return EGCInvErrorType::InvalidID;
	}

	FGCInvItemSlotData& SlotRef = ItemSlots.FindOrAdd(*InItemID);
	const uint8 PreChangeAmount = SlotRef.Amount;
	
	SlotRef.Binary = InBinary;
	SlotRef.Type = ItemData.ItemType;
	const int32 LocalOverflow = (SlotRef.Amount + InAmount) - ItemData.MaxStackSize;
	if (LocalOverflow > 0)
	{
		if (bAddUntilOverflow)
		{
			SlotRef.Amount = ItemData.MaxStackSize;
			OverflowAmount = LocalOverflow;
		}
		else
		{
			OverflowAmount = InAmount;
		}
	}
	else
	{
		SlotRef.Amount += InAmount;
		OverflowAmount = 0;
	}

	const uint8 ChangedAmount = SlotRef.Amount - PreChangeAmount;
	if (bAutoEquip && ChangedAmount == 1 && ItemData.ItemType == EGCInvItemType::Equipment
		&& (!ActiveEquipment.IsValidInfo() || GetItemData(ActiveEquipment.ItemID).EquipmentPriority > ItemData.EquipmentPriority))
	{
		EquipItem(InItemID);
	}
	
	if (!bSilent && ChangedAmount > 0)
	{
		OnItemAdded.Broadcast(InItemID, ChangedAmount);
	}

	if (!bSilent && OverflowAmount > 0)
	{
		OnItemOverflow.Broadcast(InItemID, OverflowAmount);
	}

	OnUpdate.Broadcast();
	return OverflowAmount > 0 ? EGCInvErrorType::Overflow : EGCInvErrorType::None;
}

EGCInvErrorType UGCInventoryManager::RemoveItemFromInventory(const FGCInvItemID InItemID, const uint8 InAmount, const bool bRemoveUntilEmpty, const bool bSilent, int32& MissingAmount)
{
	if (InAmount == 0)
	{
		MissingAmount = 0;
		return EGCInvErrorType::None;
	}

	const FGCInvItemData ItemData = GetItemData(InItemID);
	if (!ItemData.IsValidData())
	{
		MissingAmount = InAmount;
		return EGCInvErrorType::InvalidID;
	}

	if (*InItemID == ActiveEquipment.ItemID)
	{
		UnequipItemInternal(false);
	}

	FGCInvItemSlotData& SlotRef = ItemSlots.FindOrAdd(*InItemID);
	const uint8 PreChangeAmount = SlotRef.Amount;
	
	const int32 LocalMissing = InAmount - SlotRef.Amount;
	if (LocalMissing > 0)
	{
		if (bRemoveUntilEmpty)
		{
			SlotRef.Amount = 0;
			MissingAmount = LocalMissing;
		}
		else
		{
			MissingAmount = InAmount;
		}
	}
	else
	{
		SlotRef.Amount -= InAmount;
		MissingAmount = 0;
	}

	const uint8 ChangedAmount = PreChangeAmount - SlotRef.Amount;
	if (SlotRef.Amount == 0)
	{
		ItemSlots.Remove(*InItemID);
	}
	
	if (!bSilent && ChangedAmount > 0)
	{
		OnItemRemoved.Broadcast(InItemID, ChangedAmount);
	}

	if (!bSilent && MissingAmount > 0)
	{
		OnItemMissing.Broadcast(InItemID, MissingAmount);
	}
	
	OnUpdate.Broadcast();
	return MissingAmount > 0 ? EGCInvErrorType::Missing : EGCInvErrorType::None;
}

void UGCInventoryManager::EquipItem(const FGCInvItemID InItemID)
{
	EquipItemInternal(*InItemID, true);
}

void UGCInventoryManager::UnequipItem()
{
	UnequipItemInternal(true);
}

void UGCInventoryManager::SaveEquipmentData()
{
	if (ActiveEquipment.IsValidInfo())
	{
		ActiveEquipment.Actor->GetSaveData(ItemSlots.FindOrAdd(ActiveEquipment.ItemID).Binary);
	}
}

void UGCInventoryManager::ToggleEquipment() const
{
	if (ActiveEquipment.IsValidInfo())
	{
		ActiveEquipment.Actor->OnToggle();
	}
}

void UGCInventoryManager::SetEquipmentCharging(const bool bCharging) const
{
	if (ActiveEquipment.IsValidInfo())
	{
		if (ActiveEquipment.Actor->bCharging != bCharging)
		{
			ActiveEquipment.Actor->bCharging = bCharging;
			ActiveEquipment.Actor->OnUpdateChargeState();
		}
	}
}

void UGCInventoryManager::ConsumeItem(const FGCInvItemID InItemID)
{
	const FGCInvItemData ItemData = GetItemData(InItemID);
	if (!ItemData.IsValidData() || ItemData.ItemType != EGCInvItemType::Consumable)
		return;

	if (UGCInventoryConsumable* Object = NewObject<UGCInventoryConsumable>(this,
		ItemData.ConsumableClass.LoadSynchronous()))
	{
		Object->InventoryManager = this;
		Object->ItemID = *InItemID;
		Object->OnUseItem();

		ConsumableObjects.Add(Object);
	}
}

void UGCInventoryManager::EnsureItem(const FGCInvItemID& InItemID)
{
	if (!InItemID.IsValid())
	{
		return;
	}
	
	if (GetItemSlot(InItemID).Amount == 0)
	{
		int32 Overflow;
		AddItemToInventory(InItemID, 1, {}, false, false, true, Overflow);
	}
}

void UGCInventoryManager::LoadData(const FGCInvSaveData& InData)
{
	Currency = InData.Currency;
	ItemSlots = InData.ItemSlots;
	EquipItemInternal(InData.ActiveEquipment, false);
	GetWorld()->GetTimerManager().SetTimerForNextTick([this](){
		OnUpdate.Broadcast();
	});
}

FGCInvSaveData UGCInventoryManager::SaveData()
{
	SaveEquipmentData();
	return {Currency, ActiveEquipment.ItemID, ItemSlots};
}

void UGCInventoryManager::EquipItemInternal(const FName& InItemID, const bool bBroadcastRefresh)
{
	const FGCInvItemData ItemData = GetItemData(InItemID);
	if (!ItemData.IsValidData() || ItemData.ItemType != EGCInvItemType::Equipment)
		return;

	bool bIsUnequipping = false;
	if (ActiveEquipment.IsValidInfo())
	{
		UnequipItemInternal(false);
		bIsUnequipping = true;
	}
	
	FTimerHandle EquipTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(EquipTimerHandle, [this, InItemID, ItemData, bBroadcastRefresh](){
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (AGCInventoryEquipment* SpawnedActor = GetWorld()->SpawnActor<AGCInventoryEquipment>(ItemData.EquipmentClass.LoadSynchronous(),
			GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation(), SpawnParams))
		{
			SpawnedActor->SetActorEnableCollision(false);
			SpawnedActor->InventoryManager = this;
			SpawnedActor->LoadSaveData(ItemSlots.FindOrAdd(InItemID).Binary);
			SpawnedActor->OnEquip();

			ActiveEquipment.ItemID = InItemID;
			ActiveEquipment.Actor = SpawnedActor;
			OnEquip.Broadcast(SpawnedActor);

			if (bBroadcastRefresh)
			{
				OnUpdate.Broadcast();
			}
		}
	}, (bIsUnequipping ? 1.0f :  2.0f) * FApp::GetDeltaTime(), false);
}

void UGCInventoryManager::UnequipItemInternal(const bool bBroadcastRefresh)
{
	if (!ActiveEquipment.IsValidInfo())
		return;

	const FGCInvItemData ItemData = GetItemData(ActiveEquipment.ItemID);
	if (ItemData.IsValidData())
	{
		if (ActiveEquipment.Actor->bCharging)
		{
			ActiveEquipment.Actor->bCharging = false;
			ActiveEquipment.Actor->OnUpdateChargeState();
		}
		
		ActiveEquipment.Actor->OnUnequip();
		ActiveEquipment.Actor->GetSaveData(ItemSlots.FindOrAdd(ActiveEquipment.ItemID).Binary);
		GetWorld()->GetTimerManager().SetTimerForNextTick([this, bBroadcastRefresh](){
			OnUnequip.Broadcast(ActiveEquipment.Actor);
			ActiveEquipment.Clear();

			if (bBroadcastRefresh)
			{
				OnUpdate.Broadcast();
			}
		});
	}
}

bool UGCInventoryManager::IsValidItemID(const FGCInvItemID InItemID)
{
	if (!InItemID.IsValid())
	{
		return false;
	}
	
	if (const UDataTable* Table = UGCSettings::Get()->InventoryItemTable.LoadSynchronous())
	{
		return Table->GetRowMap().Contains(*InItemID);
	}

	return false;
}

FGCInvItemData UGCInventoryManager::GetItemData(const FGCInvItemID InItemID)
{
	if (!InItemID.IsValid())
	{
		return {};
	}
	
	if (const UDataTable* Table = UGCSettings::Get()->InventoryItemTable.LoadSynchronous())
	{
		if (const FGCInvItemData* DataPtr = Table->FindRow<FGCInvItemData>(*InItemID, StaticClass()->GetName()))
		{
			return *DataPtr;
		}
	}

	return {};
}

bool UGCInventoryManager::IsValidItemData(const FGCInvItemData& InItemData)
{
	return InItemData.IsValidData();
}

bool UGCInventoryManager::IsValidEquipment(const FGCInvEquipmentInfo& InEquipmentInfo)
{
	return InEquipmentInfo.IsValidInfo();
}
