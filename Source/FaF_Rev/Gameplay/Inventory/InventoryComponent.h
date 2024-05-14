// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Data/MiscTypes.h"
#include "InspectionActor.h"
#include "Inventory/InventoryComponentBase.h"
#include "InventoryComponent.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct GTRUNTIME_API FInventorySaveData
{
	GENERATED_BODY()

	FGuid ActiveEquipment;
	FGameCurrency MoneyData;
	TMap<FGuid, FInventorySlotData> ItemSlots;

	FInventorySaveData() : ActiveEquipment({}), ItemSlots({}) {}
	friend FArchive& operator<<(FArchive& Ar, FInventorySaveData& SaveData)
	{
		Ar << SaveData.ActiveEquipment;
		Ar << SaveData.MoneyData;
		Ar << SaveData.ItemSlots;
		return Ar;
	}
};

UCLASS(ClassGroup = (Game))
class FAF_REV_API UInventoryComponent : public UInventoryComponentBase
{
	GENERATED_BODY()
	
	friend class AFRPlayerBase;
	friend class AFRGameModeBase;
	friend class AInspectionActor;
	friend class UGameSectionManager;
	
public:

	UInventoryComponent();

	void OpenUI() const;
	void CloseUI() const;
	void EquipmentUse();
	void EquipmentUseAlt(bool bPressed);
	void SetInspectionActor(AInspectionActor* InActor);

	void LoadSaveData(const FInventorySaveData& InData);
	FInventorySaveData ExportSaveData();

protected:
	
	UPROPERTY(Transient) TObjectPtr<AFRPlayerBase> PlayerChar;
	UPROPERTY(Transient) TObjectPtr<AInspectionActor> InspectionActor;
	UPROPERTY(Transient) TObjectPtr<class UInventoryWidgetBase> InventoryWidget;
};
