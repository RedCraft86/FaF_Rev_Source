// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "GCInventoryConsumable.h"
#include "GCInventoryManager.h"

UGCInventoryConsumable::UGCInventoryConsumable()
{
	InventoryManager = nullptr;
}

void UGCInventoryConsumable::MarkAsFinished(const bool bSuccess)
{
	if (InventoryManager)
	{
		if (bSuccess)
		{
			int32 MissingCount;
			InventoryManager->RemoveItemFromInventory(ItemID, 1, false, false, MissingCount);
		}
		
		InventoryManager->ConsumableObjects.Remove(this);
		InventoryManager = nullptr;
	}
}

UWorld* UGCInventoryConsumable::GetWorld() const
{
#if WITH_EDITOR
	if (!FApp::IsGame())
	{
		return GEngine ? GEngine->GetCurrentPlayWorld() : GWorld;
	}
#endif

	const UObject* Outer = GetOuter();
	if (UWorld* OuterWorld = Outer ? Outer->GetWorld() : nullptr)
	{
		return OuterWorld;
	}

	return GEngine ? GEngine->GetCurrentPlayWorld() : GWorld;
}

void UGCInventoryConsumable::OnUseItem()
{
	EventUseItem(InventoryManager);
}
