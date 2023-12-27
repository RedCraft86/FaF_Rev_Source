// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "Engine/DataTable.h"
#include "ListGeneratorBase.h"

#include "Core/GCSettings.h"
#include "Achievement/GCAchievementTypes.h"
#include "ListGen_AchievementID.generated.h"

UCLASS(DisplayName = "List Generator - Achievement IDs")
class UAchievementIDListGen final : public UTableListGeneratorBase
{
	GENERATED_BODY()

public:

	virtual FName GetDataObjectPropertyName() const override { return GET_MEMBER_NAME_CHECKED(UGCSettings, AchievementTable); }
	virtual TSoftObjectPtr<UObject> GetDataObject() const override { return UGCSettings::Get()->AchievementTable; }

private:
	
	virtual TArray<TSharedPtr<FPulldownRow>> OnGenerate(UObject* InDataObject) const override
	{
		if (!InDataObject)
			return {};

		TArray<TSharedPtr<FPulldownRow>> Rows;
		if (const UDataTable* Table = Cast<UDataTable>(InDataObject))
		{
			Table->ForeachRow<FGCAchievementData>(GetName(),
				[&Rows](const FName& Key, const FGCAchievementData& Value)
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