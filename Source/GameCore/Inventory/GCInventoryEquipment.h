// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GCInventoryEquipment.generated.h"

UCLASS(Abstract, Blueprintable, DisplayName = "Inventory Equipment")
class GAMECORE_API AGCInventoryEquipment : public AActor
{
	GENERATED_BODY()

	friend class UGCInventoryManager;

public:
	
	AGCInventoryEquipment();

	UPROPERTY(EditDefaultsOnly, Category = "EquipmentSettings")
		bool bCanEverSave;

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Equip")
		void EventEquip(UGCInventoryManager* InManager);

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Unequip")
		void EventUnequip(UGCInventoryManager* InManager);

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Toggle")
		void EventToggle();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Charge State Changed")
		void EventUpdateChargeState(const bool bIsCharging);

	UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
		FORCEINLINE bool IsCharging() const { return bCharging; }
	
	void LoadSaveData(const TArray<uint8>& InData);
	void GetSaveData(TArray<uint8>& OutData);

protected:

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Inventory|Equipment")
		UGCInventoryManager* InventoryManager;

	bool bCharging;
	
	virtual void OnEquip();
	virtual void OnUnequip();
	virtual void OnToggle();
	virtual void OnUpdateChargeState();
};
