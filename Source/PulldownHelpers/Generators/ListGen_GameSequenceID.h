// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "Engine/DataTable.h"
#include "ListGeneratorBase.h"

#include "Core/GCSettings.h"
#include "GameSequence/GCSequenceTypes.h"
#include "ListGen_GameSequenceID.generated.h"

UCLASS(DisplayName = "List Generator - Game Sequence IDs")
class UGameSequenceIDListGen final : public UTableListGeneratorBase
{
	GENERATED_BODY()

public:

	virtual FName GetDataObjectPropertyName() const override { return GET_MEMBER_NAME_CHECKED(UGCSettings, GameSequenceTable); }
	virtual TSoftObjectPtr<UObject> GetDataObject() const override { return UGCSettings::Get()->GameSequenceTable; }

private:
	
	virtual TArray<TSharedPtr<FPulldownRow>> OnGenerate(UObject* InDataObject) const override
	{
		if (!InDataObject)
			return {};

		TArray<TSharedPtr<FPulldownRow>> Rows;
		if (const UDataTable* Table = Cast<UDataTable>(InDataObject))
		{
			Table->ForeachRow<FGCSequenceData>(GetName(),
				[&Rows](const FName& Key, const FGCSequenceData& Value)
			{
				if (Key.IsNone())
					return;

				const FText Label = Value.Label.IsEmpty() ? FText::FromName(Key) : FText::FromString(Value.Label);
				const FText Tooltip = Value.Label.IsEmpty() ? FText::GetEmpty() : FText::FromName(Key);
				Rows.Add(MakeShared<FPulldownRow>(Key.ToString(), Tooltip, Label));
			});
		}

		return Rows;
	}
};