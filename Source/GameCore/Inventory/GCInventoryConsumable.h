// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "GCInventoryConsumable.generated.h"

UCLASS(Abstract, Blueprintable, DisplayName = "Inventory Consumable")
class GAMECORE_API UGCInventoryConsumable : public UObject
{
	GENERATED_BODY()

	friend class UGCInventoryManager;

public:

	UGCInventoryConsumable();

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Consumable")
		FName ItemID;
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Use Item")
		void EventUseItem(UGCInventoryManager* InManager);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Consumable")
		void MarkAsFinished(const bool bSuccess = true);
	
	virtual UWorld* GetWorld() const override;
	
protected:

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Inventory|Consumable")
		UGCInventoryManager* InventoryManager;

	virtual void OnUseItem();
};
