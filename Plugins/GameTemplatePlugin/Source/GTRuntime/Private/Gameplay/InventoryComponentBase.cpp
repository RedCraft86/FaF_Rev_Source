// Copyright (C) RedCraft86. All Rights Reserved.

#include "Inventory/InventoryComponentBase.h"

void FInventorySlotData::ValidateMetadata()
{
	TArray<FName> Keys;
	Metadata.GenerateKeyArray(Keys);
	for (const FName& Key : Keys)
	{
		if (Metadata[Key].IsEmpty())
		{
			Metadata.Remove(Key);
		}
	}
}

bool FInventorySlotData::MatchesWith(const UInventoryItemDataBase* Item, const FInventoryItemFilter& FilterData) const
{
	if (ItemData.IsNull() || !IsValid(Item) || Item != ItemData.LoadSynchronous())
		return false;

	if (FilterData.MetaFilter == EInventoryMetaFilter::None)
		return true;

	bool bDataMatch = false;
	for (const TPair<FName, FString>& Pair : FilterData.Metadata)
	{
		if (Pair.Key.IsNone()) continue;
		const FString Value = Metadata.FindRef(Pair.Key);
		bDataMatch = Pair.Value.IsEmpty() ? !Value.IsEmpty() : Value == Pair.Value;
		switch (FilterData.MetaFilter)
		{
		case EInventoryMetaFilter::None: return true;
		case EInventoryMetaFilter::MatchAny: if (bDataMatch) return true;
		case EInventoryMetaFilter::MatchAll: if (!bDataMatch) return false;
		}
	}

	return bDataMatch;
}

UInventoryComponentBase::UInventoryComponentBase()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

FGuid UInventoryComponentBase::FindSlot(const UInventoryItemDataBase* Item, const FInventoryItemFilter& Filter)
{
	for (const TPair<FGuid, FInventorySlotData>& Slot : ItemSlots)
	{
		if (Slot.Value.MatchesWith(Item, Filter)) return Slot.Key;
	}

	return {};
}

TSet<FGuid> UInventoryComponentBase::FindSlots(const UInventoryItemDataBase* Item, const FInventoryItemFilter& Filter)
{
	TSet<FGuid> SlotKeys;
	for (const TPair<FGuid, FInventorySlotData>& Slot : ItemSlots)
	{
		if (Slot.Value.MatchesWith(Item, Filter)) SlotKeys.Add(Slot.Key);
	}

	return SlotKeys;
}

void UInventoryComponentBase::AddSlotMetadata(const FGuid& InSlot, const FName MetaKey, const FString MetaValue)
{
	if (!MetaKey.IsNone() && !MetaValue.IsEmpty() && ItemSlots.Contains(InSlot))
	{
		ItemSlots[InSlot].Metadata.Add(MetaKey, MetaValue);
		ItemSlots[InSlot].ValidateMetadata();
	}
}

void UInventoryComponentBase::AppendSlotMetadata(const FGuid& InSlot, const TMap<FName, FString>& Metadata)
{
	if (ItemSlots.Contains(InSlot))
	{
		ItemSlots[InSlot].Metadata.Append(Metadata);
		ItemSlots[InSlot].ValidateMetadata();
	}
}

void UInventoryComponentBase::RemoveSlotMetadata(const FGuid& InSlot, const FName MetaKey)
{
	if (ItemSlots.Contains(InSlot))
	{
		ItemSlots[InSlot].Metadata.Remove(MetaKey);
		ItemSlots[InSlot].ValidateMetadata();
	}
}

bool UInventoryComponentBase::SlotHasMetadata(const FGuid& InSlot, const FName MetaKey, const FString MetaValue)
{
	if (!MetaKey.IsNone() && ItemSlots.Contains(InSlot))
	{
		ItemSlots[InSlot].ValidateMetadata();
		const FString Value = ItemSlots[InSlot].Metadata.FindRef(MetaKey);
		return MetaValue.IsEmpty() ? !Value.IsEmpty() : Value == MetaValue;
	}

	return false;
}

int32 UInventoryComponentBase::AddItemToSlot(const FGuid& SlotKey, const int32 Amount, const bool bSilent)
{
	if (!ItemSlots.Contains(SlotKey) || Amount <= 0) return Amount;
	
	int32 Overflow = 0;
	ItemSlots[SlotKey].Amount += Amount;
	const UInventoryItemDataBase* Item = ItemSlots[SlotKey].ItemData.LoadSynchronous();
	const int32 StackSizeLimit = Item->GetStackLimit();
	if (ItemSlots[SlotKey].Amount > StackSizeLimit)
	{
		Overflow = ItemSlots[SlotKey].Amount - StackSizeLimit;
		ItemSlots[SlotKey].Amount = StackSizeLimit;
	}

	if (!CleanInventory()) ON_UPDATE();
	if (!bSilent) ON_ITEM_ADDED(Item, Amount - Overflow, false, ItemSlots.Contains(SlotKey) ? ItemSlots[SlotKey].Metadata : FMetadataWrapper());
	return Overflow;
}

int32 UInventoryComponentBase::RemoveItemFromSlot(const FGuid& SlotKey, const int32 Amount, const bool bSilent)
{
	if (!ItemSlots.Contains(SlotKey) || Amount <= 0) return Amount;
	
	int32 Missing = 0;
	ItemSlots[SlotKey].Amount -= Amount;
	const UInventoryItemDataBase* Item = ItemSlots[SlotKey].ItemData.LoadSynchronous();
	
	if (ItemSlots[SlotKey].Amount <= 0)
	{
		Missing = FMath::Abs(ItemSlots[SlotKey].Amount);
		ItemSlots.Remove(SlotKey);
	}

	if (!CleanInventory()) ON_UPDATE();
	if (!bSilent) ON_ITEM_REMOVED(Item, Amount - Missing, !ItemSlots.Contains(SlotKey));
	return Missing;
}

void UInventoryComponentBase::AddItemToInventory(int32& Overflow, TSet<FGuid>& Slots, const UInventoryItemDataBase* Item, const int32 Amount, const TMap<FName, FString>& Metadata, const bool bSilent)
{
	Slots = {};
	Overflow = 0;
	if (!IsValid(Item) || Amount <= 0) return;

	bool bIsNewStack = false;
	
	int32 Raw, Final;
	switch (Item->StackingMode)
	{
	case EInventoryItemStackType::Unique:
		{
			int32 Existing = FindSlots(Item).Num();
			
			Raw = Existing + Amount;
			Final = FMath::Min((int32)Item->MaxNumOfSlots, Raw);
			Overflow = FMath::Max(Raw - Final, 0);

			for (int i = 0; i < Final - Existing; i++)
			{
				const FGuid ItemGuid(FGuid::NewGuid());
				FInventorySlotData NewSlot(Item, 1, Metadata);
				ItemSlots.Add(ItemGuid, NewSlot);
				Slots.Add(ItemGuid);
			}

			bIsNewStack = true;
		}
		break;
		
	default:
		{
			if (const FGuid Slot = FindSlot(Item); Slot.IsValid())
			{
				Raw = ItemSlots[Slot].Amount + Amount;
				Final = FMath::Min(Item->GetStackLimit(), Raw);
				Overflow = FMath::Max(Raw - Final, 0);
				
				ItemSlots[Slot].Amount = Final;
				ItemSlots[Slot].Metadata.Append(Metadata);
				ItemSlots[Slot].ValidateMetadata();
			}
			else
			{
				Raw = Amount;
				Final = FMath::Min(Item->GetStackLimit(), Raw);
				Overflow = FMath::Max(Raw - Final, 0);

				const FGuid ItemGuid(FGuid::NewGuid());
				FInventorySlotData NewSlot(Item, Final, Metadata);
				ItemSlots.Add(ItemGuid, NewSlot);
				Slots.Add(ItemGuid);
				
				bIsNewStack = true;
			}
		}
		break;
	}

	if (Final <= 0) return;
	if (!CleanInventory()) ON_UPDATE();
	if (!bSilent) ON_ITEM_ADDED(Item, Final, bIsNewStack, {Metadata});
}

int32 UInventoryComponentBase::AddItem(const UInventoryItemDataBase* Item, const int32 Amount, const TMap<FName, FString>& Metadata, const bool bSilent)
{
	int32 Overflow;
	TSet<FGuid> Slots;
	AddItemToInventory(Overflow, Slots, Item, Amount, Metadata, bSilent);
	return Overflow;
}

void UInventoryComponentBase::RemoveItemFromInventory(int32& Missing, const UInventoryItemDataBase* Item, const int32 Amount, const FInventoryItemFilter& Filter, const bool bSilent)
{
	Missing = 0;
	if (!IsValid(Item) || Amount <= 0) return;

	bool bIsEmptyStack = false;
	
	int32 Raw, Final;
	switch (Item->StackingMode)
	{
	case EInventoryItemStackType::Unique:
		{
			const TSet<FGuid> Slots = FindSlots(Item, Filter);
			Raw = Slots.Num() - Amount;
			Missing = Raw < 0 ? FMath::Abs(Raw) : 0;
			Final = Amount - Missing;
			
			Raw = Final;
			for (const FGuid& Slot : Slots)
			{
				Raw--;
				ItemSlots.Remove(Slot);
				if (Raw <= 0) break;
			}

			bIsEmptyStack = true;
		}
		break;
		
	default:
		{
			if (const FGuid Slot = FindSlot(Item); Slot.IsValid())
			{
				Raw = ItemSlots[Slot].Amount - Amount;
				Missing = Raw < 0 ? FMath::Abs(Raw) : 0;
				Final = Amount - Missing;

				ItemSlots[Slot].Amount -= Final;
				if (ItemSlots[Slot].Amount <= 0)
				{
					ItemSlots.Remove(Slot);
					bIsEmptyStack = true;
				}
			}
			else
			{
				Missing = Amount;
				Final = 0;
			}
		}
		break;
	}

	if (Final <= 0) return;
	if (!CleanInventory()) ON_UPDATE();
	if (!bSilent) ON_ITEM_REMOVED(Item, Final, bIsEmptyStack);
}

int32 UInventoryComponentBase::RemoveItem(const UInventoryItemDataBase* Item, const int32 Amount, const FInventoryItemFilter& Filter, const bool bSilent)
{
	int32 Missing;
	RemoveItemFromInventory(Missing, Item, Amount, Filter, bSilent);
	return Missing;
}

bool UInventoryComponentBase::CleanInventory()
{
	TArray<FGuid> Slots;
	ItemSlots.GenerateKeyArray(Slots);

	bool bHasAnyChange = false;
	for (const FGuid& Slot : Slots)
	{
		if (!ItemSlots[Slot].IsValidSlot())
		{
			bHasAnyChange = true;
			ItemSlots.Remove(Slot);
		}
	}

	if (bHasAnyChange) ON_UPDATE();
	return bHasAnyChange;
}
