// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "Engine/DataTable.h"
#include "ListGeneratorBase.h"

#include "Core/GCSettings.h"
#include "Inventory/GCInventoryTypes.h"
#include "ListGen_InventoryItemID.generated.h"

UCLASS(DisplayName = "List Generator - Inventory Item IDs")
class UInventoryItemIDListGen final : public UTableListGeneratorBase
{
	GENERATED_BODY()

public:

	virtual FName GetDataObjectPropertyName() const override { return GET_MEMBER_NAME_CHECKED(UGCSettings, InventoryItemTable); }
	virtual TSoftObjectPtr<UObject> GetDataObject() const override { return UGCSettings::Get()->InventoryItemTable; }

private:
	
	virtual TArray<TSharedPtr<FPulldownRow>> OnGenerate(UObject* InDataObject) const override
	{
		if (!InDataObject)
			return {};

		TArray<TSharedPtr<FPulldownRow>> Rows;
		if (const UDataTable* Table = Cast<UDataTable>(InDataObject))
		{
			Table->ForeachRow<FGCInvItemData>(GetName(),
				[&Rows](const FName& Key, const FGCInvItemData& Value)
			{
				if (Key.IsNone())
					return;

				const FText Label = Value.DisplayName.IsEmpty() ? FText::FromName(Key) : Value.DisplayName;
				const FText Tooltip = Value.DisplayName.IsEmpty() ? FText::GetEmpty() : FText::FromName(Key);
				Rows.Add(MakeShared<FPulldownRow>(Key.ToString(), Tooltip, Label));
			});
		}

		return Rows;
	}
};