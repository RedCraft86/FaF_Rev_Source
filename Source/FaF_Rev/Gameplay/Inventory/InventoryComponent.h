// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "EquipmentActor.h"
#include "InventoryPreview.h"
#include "InventoryItemData.h"
#include "Inventory/InventoryComponentBase.h"
#include "InventoryComponent.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct FAF_REV_API FInventorySaveData
{
	GENERATED_BODY()

	UPROPERTY() FGuid ActiveEquipment;
	UPROPERTY() FGameCurrency CurrencyData;

	FInventorySaveData() : ActiveEquipment({}), CurrencyData({}) {}
	FInventorySaveData(const FGuid InEquipment, const FGameCurrency InCurrency)
		: ActiveEquipment(InEquipment), CurrencyData(InCurrency)
	{}
	
	friend FArchive& operator<<(FArchive& Ar, FInventorySaveData& SaveData)
	{
		Ar << SaveData.ActiveEquipment;
		Ar << SaveData.CurrencyData;
		return Ar;
	}
};

USTRUCT(BlueprintInternalUseOnly)
struct FAF_REV_API FInventoryEquipmentData
{
	GENERATED_BODY()

	UPROPERTY() FGuid ItemID;
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
	void CloseUI();
	bool IsInInventory() const { return bIsInInventory; }
	
	void EquipmentUse() const;
	void EquipmentUseAlt(bool bPressed) const;
	void SetInventoryPreview(AInventoryPreview* InActor);
	AInventoryPreview* GetInventoryPreview() const { return InventoryPreview; }
	TArray<FGuid> GetSortedSlots(const EInventoryItemType TypeFilter = EInventoryItemType::Any);

	bool UseKeyItem(const UInventoryItemDataBase* InItem, const FString& KeyID);
	void ConsumeItem(const FGuid& ItemKey);
	
	void UnequipItem();
	void EquipItem(const FGuid& ItemKey);
	const FInventoryEquipmentData& GetEquipmentData() const { return EquipmentData; }
		
	void ImportSaveData(const FInventorySaveData& InData);
	FInventorySaveData ExportSaveData();

protected:

	UPROPERTY() FTimerHandle EquipTimer;
	UPROPERTY() bool bIsInInventory = false;
	UPROPERTY(Transient) TObjectPtr<AFRPlayerBase> PlayerChar;
	UPROPERTY(Transient) TObjectPtr<AInventoryPreview> InventoryPreview;
	UPROPERTY(Transient) TObjectPtr<class UInventoryWidgetBase> InventoryWidget;
	UPROPERTY(Transient) FInventoryEquipmentData EquipmentData;
};
