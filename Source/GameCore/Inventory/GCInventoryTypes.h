// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "RCMathTypes.h"
#include "Core/CoreMacros.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "GCInventoryEquipment.h"
#include "GCInventoryConsumable.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "GCInventoryTypes.generated.h"

UENUM(BlueprintType, DisplayName = "Inventory Error Type")
enum class EGCInvErrorType : uint8
{
	None,
	InvalidID,
	Overflow,
	Missing,
};

UENUM(BlueprintType, DisplayName = "Inventory Item Type")
enum class EGCInvItemType : uint8
{
	Basic,
	KeyItem,
	Viewable,
	Equipment,
	Consumable
};

USTRUCT(BlueprintType, DisplayName = "Inventory Item ID")
struct GAMECORE_API FGCInvItemID : public FPulldownStructBase
{
	GENERATED_BODY()
	SETUP_PULLDOWN(FGCInvItemID);
};
SETUP_PULLDOWN_HASH(FGCInvItemID);

USTRUCT(BlueprintType, DisplayName = "Inventory Item Data")
struct GAMECORE_API FGCInvItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemSettings")
		uint8 Priority;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemSettings")
		TSoftObjectPtr<UTexture2D> IconImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemSettings")
		FText DisplayName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemSettings", meta = (MultiLine = true))
		FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemSettings", meta = (ClampMin = 1, UIMin = 1))
		uint8 MaxStackSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PreviewSettings")
		TSoftObjectPtr<UStaticMesh> PreviewMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PreviewSettings")
		TArray<TSoftObjectPtr<UMaterialInterface>> PreviewMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PreviewSettings")
		bool bPreviewCastShadows;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PreviewSettings")
		FTransform PreviewTransform;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PreviewSettings", meta = (ClampMin = 0.25f, UIMin = 0.25f, ClampMax = 2.0f, UIMax = 2.0f))
		FVector2D PreviewZoomRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TypeSettings", meta = (InlineCategoryProperty))
		EGCInvItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TypeSettings", meta = (EditCondition = "ItemType == EGCInvItemType::KeyItem", EditConditionHides))
		FGameplayTag KeyTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TypeSettings", meta = (EditCondition = "ItemType == EGCInvItemType::Viewable", EditConditionHides))
		TSoftObjectPtr<UTexture2D> ViewImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TypeSettings", meta = (EditCondition = "ItemType == EGCInvItemType::Viewable", EditConditionHides, MultiLine = true))
		FText ViewText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TypeSettings", meta = (EditCondition = "ItemType == EGCInvItemType::Equipment", EditConditionHides, AllowAbstract = false))
		TSoftClassPtr<AGCInventoryEquipment> EquipmentClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TypeSettings", meta = (EditCondition = "ItemType == EGCInvItemType::Equipment", EditConditionHides))
		uint8 EquipmentPriority;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TypeSettings", meta = (EditCondition = "ItemType == EGCInvItemType::Consumable", EditConditionHides, AllowAbstract = false))
		TSoftClassPtr<UGCInventoryConsumable> ConsumableClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TypeSettings", meta = (EditCondition = "ItemType == EGCInvItemType::Consumable", EditConditionHides))
		FText UseDisplayText;

	FGCInvItemData()
		: Priority(1)
		, IconImage(nullptr)
		, DisplayName(FText::FromString("Invalid Item"))
		, Description(FText::FromString("This item is Invalid!"))
		, MaxStackSize(10)
		, PreviewMesh(nullptr)
		, PreviewMaterials({})
		, bPreviewCastShadows(false)
		, PreviewTransform(FTransform::Identity)
		, PreviewZoomRange(FVector2D(0.5, 1.25))
		, ItemType(EGCInvItemType::Basic)
		, ViewImage(nullptr)
		, ViewText(FText::FromString("None"))
		, EquipmentClass(NULL)
		, EquipmentPriority(0)
		, ConsumableClass(NULL)
		, UseDisplayText(FText::FromString("Consume"))
	{}

	FString GetTypeString() const
	{
		switch (ItemType)
		{
		case EGCInvItemType::KeyItem:
			return TEXT("Key/Important");

		case EGCInvItemType::Viewable:
			return TEXT("Photo/Letter");

		case EGCInvItemType::Equipment:
			return TEXT("Equipment");

		case EGCInvItemType::Consumable:
			return TEXT("Consumable");

		default: return TEXT("Basic");
		}
	}

	bool IsValidData() const
	{
		bool bTypeCondition;
		switch (ItemType)
		{
		case EGCInvItemType::Viewable:
			bTypeCondition = !ViewImage.IsNull() || !ViewText.IsEmptyOrWhitespace();
			break;

		case EGCInvItemType::Equipment:
			bTypeCondition = !EquipmentClass.IsNull();
			break;

		case EGCInvItemType::Consumable:
			bTypeCondition = !ConsumableClass.IsNull();
			break;

		default: bTypeCondition = true;
			break;
		}
		
		return !IconImage.IsNull() && !DisplayName.IsEmptyOrWhitespace() && MaxStackSize > 0 && bTypeCondition;
	}

#if WITH_EDITOR
private:
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		Super::OnDataTableChanged(InDataTable, InRowName);

		if (Priority == 0)
		{
			InDataTable->ForeachRow<FGCInvItemData>(InDataTable->GetName(),
				[this, InRowName](const FName& Key, const FGCInvItemData& Value){
				if (Value.Priority == 0 && Key != InRowName)
				{
					Priority = 1;
				}
			});
		}
		
		if (DisplayName.IsEmptyOrWhitespace())
		{
			DisplayName = FText::FromName(InRowName);
		}
		
		if (Description.IsEmptyOrWhitespace())
		{
			Description = FText::FromString(InRowName.ToString() + TEXT(" Description"));
		}

		if (UseDisplayText.IsEmptyOrWhitespace())
		{
			UseDisplayText = FText::FromString("Consume");
		}

		if (const UStaticMesh* LoadedMesh = PreviewMesh.LoadSynchronous())
		{
			const TArray<FStaticMaterial>& Mats = LoadedMesh->GetStaticMaterials();
			if (PreviewMaterials.IsEmpty() || PreviewMaterials.Num() < Mats.Num())
			{
				for (int32 i = 0; i < Mats.Num(); i++)
				{
					if (!PreviewMaterials.IsValidIndex(i))
					{
						PreviewMaterials.Add(Mats[i].MaterialInterface);
					}
					else if (!PreviewMaterials[i])
					{
						PreviewMaterials[i] = Mats[i].MaterialInterface;
					}
				}
			}
		}
		else
		{
			PreviewMaterials.Empty();
		}

		if (PreviewZoomRange.X > PreviewZoomRange.Y)
		{
			PreviewZoomRange.X = PreviewZoomRange.Y;
		}
	}
#endif
};

USTRUCT(BlueprintType, DisplayName = "Inventory Item Slot")
struct GAMECORE_API FGCInvItemSlotData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemSlot")
		EGCInvItemType Type;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemSlot")
		uint8 Amount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemSlot")
		TArray<uint8> Binary;

	FGCInvItemSlotData() : Type(EGCInvItemType::Basic), Amount(0), Binary({}) {}
	friend FArchive& operator<<(FArchive& Ar, FGCInvItemSlotData& SlotData)
	{
		Ar << SlotData.Type;
		Ar << SlotData.Amount;
		Ar << SlotData.Binary;
		return Ar;
	}
};

USTRUCT(BlueprintType, DisplayName = "Inventory Equipment Info")
struct GAMECORE_API FGCInvEquipmentInfo
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EquipmentInfo")
		FName ItemID;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly, Category = "EquipmentInfo")
		AGCInventoryEquipment* Actor;

	FGCInvEquipmentInfo() : ItemID(NAME_None), Actor(nullptr) {}

	void Clear()
	{
		ItemID = NAME_None;
		Actor = nullptr;
	}
	
	FORCEINLINE bool IsValidInfo() const
	{
		return !ItemID.IsNone() && IsValid(Actor);
	}
};

USTRUCT(BlueprintType)
struct GAMECORE_API FGCInvSaveData
{
	GENERATED_BODY();
	
	FRCCurrency Currency;
	FName ActiveEquipment;
	TMap<FName, FGCInvItemSlotData> ItemSlots;

	FGCInvSaveData() : Currency(0), ActiveEquipment(NAME_None), ItemSlots({}) {}
	FGCInvSaveData(const FRCCurrency& InCurrency, const FName& InActiveEquipment, const TMap<FName, FGCInvItemSlotData>& InItemSlots)
		: Currency(InCurrency)
		, ActiveEquipment(InActiveEquipment)
		, ItemSlots(InItemSlots)
	{}
	
	friend FArchive& operator<<(FArchive& Ar, FGCInvSaveData& SaveData)
	{
		Ar << SaveData.Currency;
		Ar << SaveData.ItemSlots;
		Ar << SaveData.ActiveEquipment;
		return Ar;
	}
};