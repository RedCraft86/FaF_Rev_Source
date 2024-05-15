// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "EquipmentActor.h"
#include "InventoryPreview.h"
#include "Inventory/InventoryComponentBase.h"
#include "InventoryComponent.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct FAF_REV_API FInventorySaveData
{
	GENERATED_BODY()

	FGuid ActiveEquipment;
	FGameCurrency CurrencyData;
	TMap<FGuid, FInventorySlotData> ItemSlots;

	FInventorySaveData() : ActiveEquipment({}), CurrencyData({}), ItemSlots({}) {}
	FInventorySaveData(const FGuid InEquipment, const FGameCurrency InCurrency, const TMap<FGuid, FInventorySlotData>& InSlots)
		: ActiveEquipment(InEquipment), CurrencyData(InCurrency), ItemSlots(InSlots)
	{}
	
	friend FArchive& operator<<(FArchive& Ar, FInventorySaveData& SaveData)
	{
		Ar << SaveData.ActiveEquipment;
		Ar << SaveData.CurrencyData;
		Ar << SaveData.ItemSlots;
		return Ar;
	}
};

USTRUCT(BlueprintInternalUseOnly)
struct FAF_REV_API FInventoryEquipmentData
{
	GENERATED_BODY()

	FGuid ItemID;
	UPROPERTY(Transient) TObjectPtr<AEquipmentActor> Equipment;
	FInventoryEquipmentData() : ItemID({}), Equipment(nullptr) {}
};

UCLASS(ClassGroup = (Game))
class FAF_REV_API UInventoryComponent : public UInventoryComponentBase
{
	GENERATED_BODY()
	
	friend class AFRPlayerBase;
	friend class AFRGameModeBase;
	friend class AInventoryPreview;
	friend class UGameSectionManager;
	friend class UInventorySlotWidgetBase;
	
public:

	UInventoryComponent();

	void OpenUI();
	void CloseUI() const;
	void EquipmentUse() const;
	void EquipmentUseAlt(bool bPressed) const;
	void SetInventoryPreview(AInventoryPreview* InActor);
	AInventoryPreview* GetInventoryPreview() const { return InventoryPreview; }
	TArray<FGuid> GetSortedSlots();
	
	void UnequipItem();
	void EquipItem(const FGuid& ItemKey);
	void ConsumeItem(const FGuid& ItemKey);
	const FInventoryEquipmentData& GetEquipmentData() const { return EquipmentData; }
		
	void LoadSaveData(const FInventorySaveData& InData);
	FInventorySaveData ExportSaveData();

protected:
	
	UPROPERTY(Transient) TObjectPtr<AFRPlayerBase> PlayerChar;
	UPROPERTY(Transient) TObjectPtr<AInventoryPreview> InventoryPreview;
	UPROPERTY(Transient) TObjectPtr<class UInventoryWidgetBase> InventoryWidget;
	UPROPERTY(Transient) FInventoryEquipmentData EquipmentData;
};
