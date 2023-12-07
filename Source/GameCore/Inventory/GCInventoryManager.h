// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "RCMathTypes.h"
#include "GCInventoryTypes.h"
#include "Components/ActorComponent.h"
#include "GCInventoryManager.generated.h"

UCLASS(DisplayName = "Inventory Manager", NotBlueprintable, ClassGroup = (Gameplay), meta = (BlueprintSpawnableComponent))
class GAMECORE_API UGCInventoryManager final : public UActorComponent
{
	GENERATED_BODY()

	friend class UGCInventoryConsumable;
		
public:
	
	UGCInventoryManager();

	static UGCInventoryManager* Get(const UObject* WorldContext);
	
	UFUNCTION(BlueprintPure, Category = "Inventory|Manager")
		FRCCurrency GetCurrency() const { return Currency; }

	UFUNCTION(BlueprintCallable, Category = "Inventory|Manager")
		void SetCurrency(const FRCCurrency& InCurrency) { Currency = InCurrency; }

	UFUNCTION(BlueprintPure, Category = "Inventory|Manager")
		TMap<FName, FGCInvItemSlotData> GetItemSlots() const { return ItemSlots; }

	UFUNCTION(BlueprintPure, Category = "Inventory|Manager")
		TMap<FName, FGCInvItemSlotData> GetItemSlotsByType(const EGCInvItemType InType) const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Manager")
		TArray<FName> GetSortedItemIDs() const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Manager")
		TArray<FName> GetSortedItemIDsByType(const EGCInvItemType InType) const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Manager")
		bool HaveItemSlot(const FGCInvItemID InItemID) const;
	
	UFUNCTION(BlueprintPure, Category = "Inventory|Manager")
		FGCInvItemSlotData GetItemSlot(const FGCInvItemID InItemID) const;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory|Manager", meta = (InAmount = 1, AutoCreateRefTerm = "InBinary"))
		EGCInvErrorType AddItemToInventory(const FGCInvItemID InItemID, const uint8 InAmount, const TArray<uint8>& InBinary, const bool bAutoEquip, const bool bAddUntilOverflow, const bool bSilent,  int32& OverflowAmount);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Manager", meta = (InAmount = 1))
		EGCInvErrorType RemoveItemFromInventory(const FGCInvItemID InItemID, const uint8 InAmount, const bool bRemoveUntilEmpty, const bool bSilent, int32& MissingAmount);

	UFUNCTION(BlueprintPure, Category = "Inventory|Manager")
		FGCInvEquipmentInfo GetActiveEquipment() const { return ActiveEquipment; }

	UFUNCTION(BlueprintCallable, Category = "Inventory|Manager")
		void EquipItem(const FGCInvItemID InItemID);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Manager")
		void UnequipItem();

	UFUNCTION(BlueprintCallable, Category = "Inventory|Manager")
		void SaveEquipmentData();

	UFUNCTION(BlueprintCallable, Category = "Inventory|Manager")
		void ToggleEquipment() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Manager")
		void SetEquipmentCharging(const bool bCharging) const;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory|Manager")
		void ConsumeItem(const FGCInvItemID InItemID);
	
	DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdate)
	FOnInventoryUpdate OnUpdate;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryEquipUpdate, AActor*)
	FOnInventoryEquipUpdate OnEquip;
	FOnInventoryEquipUpdate OnUnequip;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventorySlotUpdate, const FGCInvItemID&, const uint8 Amount)
	FOnInventorySlotUpdate OnItemAdded;
	FOnInventorySlotUpdate OnItemRemoved;
	FOnInventorySlotUpdate OnItemOverflow;
	FOnInventorySlotUpdate OnItemMissing;

	const TMap<FName, FGCInvItemSlotData>& GetItemSlotsConst() const { return ItemSlots; }
	void EnsureItem(const FGCInvItemID& InItemID);
	void LoadData(const FGCInvSaveData& InData);
	FGCInvSaveData SaveData();
	
protected:

	UPROPERTY(Transient)
		FGCInvEquipmentInfo ActiveEquipment;
	
	UPROPERTY(Transient)
		TSet<UGCInventoryConsumable*> ConsumableObjects;
	
	FRCCurrency Currency;
	TMap<FName, FGCInvItemSlotData> ItemSlots;

	void EquipItemInternal(const FName& InItemID, const bool bBroadcastRefresh);
	void UnequipItemInternal(const bool bBroadcastRefresh);

public: // Static functions
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
		static bool IsValidItemID(const FGCInvItemID InItemID);
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
		static FGCInvItemData GetItemData(const FGCInvItemID InItemID);

	UFUNCTION(BlueprintPure, Category = "Inventory")
		static bool IsValidItemData(const FGCInvItemData& InItemData);

	UFUNCTION(BlueprintPure, Category = "Inventory")
		static bool IsValidEquipment(const FGCInvEquipmentInfo& InEquipmentInfo);
};
