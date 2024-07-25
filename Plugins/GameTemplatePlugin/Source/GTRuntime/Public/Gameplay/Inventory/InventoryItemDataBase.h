// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "InventoryItemDataBase.generated.h"

#define ITEM_LIMIT_MAX 9999

UENUM(BlueprintType)
enum class EInventoryItemStackType : uint8
{
	UntilMax,
	Defined,
	Unique
};

UCLASS(BlueprintType)
class GTRUNTIME_API UInventoryItemDataBase : public UDataAsset
{
	GENERATED_BODY()

public:

	UInventoryItemDataBase()
		: UniqueID(FGuid::NewGuid()), DisplayIcon(nullptr)
		, StackingMode(EInventoryItemStackType::UntilMax)
		, MaxStackSize(10), MaxNumOfSlots(1)
	{}

	static inline TSubclassOf<UInventoryItemDataBase> DefaultSpawnClass = StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData", meta = (DisplayPriority = -10))
		FGuid UniqueID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData", meta = (DisplayPriority = -10))
		TSoftObjectPtr<UTexture2D> DisplayIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData", meta = (DisplayPriority = -9))
		EInventoryItemStackType StackingMode;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData", meta = (DisplayPriority = -9, ClampMin = 2, UIMin = 2, EditCondition = "StackingMode == EInventoryItemStackType::Defined", EditConditionHides))
		uint8 MaxStackSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData", meta = (DisplayPriority = -9, ClampMin = 1, UIMin = 1, EditCondition = "StackingMode == EInventoryItemStackType::Unique", EditConditionHides))
		uint8 MaxNumOfSlots;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData", meta = (DisplayPriority = -8, MultiLine = true))
		TMap<FName, FString> DefaultMetadata;

	int32 GetStackLimit() const
	{
		switch (StackingMode)
		{
		case EInventoryItemStackType::UntilMax: return ITEM_LIMIT_MAX;
		case EInventoryItemStackType::Defined: return MaxStackSize;
		case EInventoryItemStackType::Unique: return 1;
		default: return ITEM_LIMIT_MAX;
		}
	}
};
