// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Data/MiscTypes.h"
#include "InventoryItemDataBase.h"
#include "Components/ActorComponent.h"
#include "InventoryComponentBase.generated.h"

UENUM(BlueprintType)
enum class EInventoryMetaFilter : uint8
{
	None,
	MatchAll,
	MatchAny
};

USTRUCT(BlueprintType)
struct GTRUNTIME_API FInventoryItemFilter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlotData")
		EInventoryMetaFilter MetaFilter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlotData", meta = (EditCondition = "MetaFilter != EInventoryMetaFilter::None"))
		TMap<FName, FString> Metadata;
	
	FInventoryItemFilter() : MetaFilter(EInventoryMetaFilter::None), Metadata({}) {}
};

USTRUCT(BlueprintType)
struct GTRUNTIME_API FInventorySlotData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SlotData", meta = (DisplayThumbnail = false))
		TSoftObjectPtr<UInventoryItemDataBase> ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SlotData", meta = (ClampMin = 0, UIMin = 0))
		int32 Amount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SlotData", meta = (MultiLine = true))
		TMap<FName, FString> Metadata;

	FInventorySlotData() : ItemData(nullptr), Amount(0) {}
	explicit FInventorySlotData(const UInventoryItemDataBase* Data) : ItemData(Data), Amount(1), Metadata({})
	{
		if (Data) Metadata.Append(Data->DefaultMetadata);
		ValidateMetadata();
	}
	
	explicit FInventorySlotData(const UInventoryItemDataBase* Data, const int32 Amount,
		const TMap<FName, FString>& InMetadata = {}) : ItemData(Data), Amount(Amount)
	{
		if (Data) Metadata.Append(Data->DefaultMetadata);
		Metadata.Append(InMetadata);
		ValidateMetadata();
	}
	
	friend FArchive& operator<<(FArchive& Ar, FInventorySlotData& SlotData)
	{
		Ar << SlotData.ItemData;
		Ar << SlotData.Amount;
		Ar << SlotData.Metadata;
		return Ar;
	}

	template <typename T = UInventoryItemDataBase>
	T* GetItemData() const { return Cast<T>(ItemData.LoadSynchronous()); }

	void ValidateMetadata();
	bool IsValidSlot() const { return !ItemData.IsNull() && Amount > 0; }
	bool MatchesWith(const UInventoryItemDataBase* Item, const FInventoryItemFilter& FilterData) const;
};

USTRUCT(BlueprintType)
struct FMetadataWrapper
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metadata")
		TMap<FName, FString> Metadata;

	FMetadataWrapper() : Metadata({}) {}
	FMetadataWrapper(const TMap<FName, FString>& InMetadata) : Metadata(InMetadata) {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryUpdateSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FInventoryIOUpdateSignature, const UInventoryItemDataBase*, Item, const int32, Amount, const bool, bNewOrEmpty, const FMetadataWrapper&, Metadata);

#define ON_UPDATE() { OnInventoryUpdate(); OnUpdate.Broadcast(); OnUpdateBP.Broadcast(); }
#define ON_ITEM_ADDED(Item, Amount, New, Meta) { OnItemAdded.Broadcast(Item, Amount, New, Meta); OnItemAddedBP.Broadcast(Item, Amount, New, Meta); }
#define ON_ITEM_REMOVED(Item, Amount, Empty) { OnItemRemoved.Broadcast(Item, Amount, Empty, {}); OnItemRemovedBP.Broadcast(Item, Amount, Empty, {}); }

UCLASS(Abstract)
class GTRUNTIME_API UInventoryComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:

	UInventoryComponentBase();

	UFUNCTION(BlueprintPure, Category = "InventoryManager")
		virtual const TMap<FGuid, FInventorySlotData>& GetInventory() const { return ItemSlots; }
	virtual TMap<FGuid, FInventorySlotData>& GetInventoryRef() { return ItemSlots; }

	UFUNCTION(BlueprintPure, Category = "InventoryManager")
		virtual bool IsValidSlot(const FGuid& InSlot) const { return InSlot.IsValid() && ItemSlots.Contains(InSlot); }
	
	UFUNCTION(BlueprintPure, Category = "InventoryManager", meta = (AdvancedDisplay = "Filter"))
		virtual FGuid FindSlot(const UInventoryItemDataBase* Item, const FInventoryItemFilter& Filter = FInventoryItemFilter());

	UFUNCTION(BlueprintPure, Category = "InventoryManager", meta = (AdvancedDisplay = "Filter"))
		virtual TSet<FGuid> FindSlots(const UInventoryItemDataBase* Item, const FInventoryItemFilter& Filter = FInventoryItemFilter());

	UFUNCTION(BlueprintCallable, Category = "InventoryManager")
		virtual void AddSlotMetadata(const FGuid& InSlot, const FName MetaKey, const FString MetaValue);

	UFUNCTION(BlueprintCallable, Category = "InventoryManager")
		virtual void AppendSlotMetadata(const FGuid& InSlot, const TMap<FName, FString>& Metadata);
	
	UFUNCTION(BlueprintCallable, Category = "InventoryManager")
		virtual void RemoveSlotMetadata(const FGuid& InSlot, const FName MetaKey);

	UFUNCTION(BlueprintCallable, Category = "InventoryManager")
		virtual bool SlotHasMetadata(const FGuid& InSlot, const FName MetaKey, const FString MetaValue = TEXT(""));
	
	UFUNCTION(BlueprintCallable, Category = "InventoryManager", meta = (AdvancedDisplay = "Filter,bSilent"))
		virtual int32 AddItemToSlot(const FGuid& SlotKey, const int32 Amount, const bool bSilent = false);

	UFUNCTION(BlueprintCallable, Category = "InventoryManager", meta = (AdvancedDisplay = "Filter,bSilent"))
		virtual int32 RemoveItemFromSlot(const FGuid& SlotKey, const int32 Amount, const bool bSilent = false);
	
	UFUNCTION(BlueprintCallable, Category = "InventoryManager", meta = (AdvancedDisplay = "Filter,bSilent"))
		virtual void AddItemToInventory(int32& Overflow, TSet<FGuid>& Slots, const UInventoryItemDataBase* Item, const int32 Amount, const TMap<FName, FString>& Metadata, const bool bSilent = false);
	int32 AddItem(const UInventoryItemDataBase* Item, const int32 Amount, const TMap<FName, FString>& Metadata, const bool bSilent = false);
	
	UFUNCTION(BlueprintCallable, Category = "InventoryManager", meta = (AdvancedDisplay = "Filter,bSilent"))
		virtual void RemoveItemFromInventory(int32& Missing, const UInventoryItemDataBase* Item, const int32 Amount, const FInventoryItemFilter& Filter = FInventoryItemFilter(), const bool bSilent = false);
	int32 RemoveItem(const UInventoryItemDataBase* Item, const int32 Amount, const FInventoryItemFilter& Filter = {}, const bool bSilent = false);

	DECLARE_MULTICAST_DELEGATE(FInventoryUpdateEvent);
	FInventoryUpdateEvent OnUpdate;
	
	DECLARE_MULTICAST_DELEGATE_FourParams(FInventoryIOUpdateEvent, const UInventoryItemDataBase*, const int32, const bool, const FMetadataWrapper&);
	FInventoryIOUpdateEvent OnItemAdded, OnItemRemoved;
	
protected:

	UPROPERTY(BlueprintAssignable, DisplayName = "On Update")
		FInventoryUpdateSignature OnUpdateBP;
	
	UPROPERTY(BlueprintAssignable, DisplayName = "On Item Added")
		FInventoryIOUpdateSignature OnItemAddedBP;
	
	UPROPERTY(BlueprintAssignable, DisplayName = "On Item Removed")
		FInventoryIOUpdateSignature OnItemRemovedBP;

	UPROPERTY() FGameCurrency CurrencyData;
	UPROPERTY() TMap<FGuid, FInventorySlotData> ItemSlots;

	bool CleanInventory();
	virtual void OnInventoryUpdate() {}
};
