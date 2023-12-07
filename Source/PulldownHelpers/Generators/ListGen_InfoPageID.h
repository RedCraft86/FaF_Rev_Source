// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "Engine/DataTable.h"
#include "ListGeneratorBase.h"

#include "Core/GCSettings.h"
#include "UserInterface/Messaging/GCInfoBookWidget.h"
#include "ListGen_InfoPageID.generated.h"

UCLASS(DisplayName = "List Generator - Info Page IDs")
class UInfoPageIDListGen final : public UTableListGeneratorBase
{
	GENERATED_BODY()

public:

	virtual FName GetDataObjectPropertyName() const override { return GET_MEMBER_NAME_CHECKED(UGCSettings, InfoPageTable); }
	virtual TSoftObjectPtr<UObject> GetDataObject() const override { return UGCSettings::Get()->InfoPageTable; }

private:
	
	virtual TArray<TSharedPtr<FPulldownRow>> OnGenerate(UObject* InDataObject) const override
	{
		if (!InDataObject)
			return {};

		TArray<TSharedPtr<FPulldownRow>> Rows;
		if (const UDataTable* Table = Cast<UDataTable>(InDataObject))
		{
			Table->ForeachRow<FGCInfoPageData>(GetName(),
				[&Rows](const FName& Key, const FGCInfoPageData& Value)
			{
				if (Key.IsNone())
					return;

				const FText Label = Value.Label.IsEmpty() ? FText::FromName(Key) : Value.Label;
				const FText Tooltip = Value.Label.IsEmpty() ? FText::GetEmpty() : FText::FromName(Key);
				Rows.Add(MakeShared<FPulldownRow>(Key.ToString(), Tooltip, Label));
			});
		}

		return Rows;
	}
};