// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "FRSettings.h"
#include "MessagingData.h"
#include "Engine/DataTable.h"
#include "ListGeneratorBase.h"
#include "ListGen_GuideBookPage.generated.h"

UCLASS(DisplayName = "[] List Generator - Guide Book Page IDs")
class UListGen_GuideBookPage final : public UTableListGenerator
{
	GENERATED_BODY()

public:

	virtual FName GetDataObjectSettingName() const override { return GET_MEMBER_NAME_CHECKED(UFRSettings, GuideTable); }
	virtual TSoftObjectPtr<UObject> GetDataObject() const override { return FRSettings->GuideTable; }

private:
	
	virtual TArray<TSharedPtr<FPulldownRow>> OnGenerate() const override
	{
		TArray<TSharedPtr<FPulldownRow>> Rows;
		if (GetDataObj(const UDataTable, Table))
		{
			Table->ForeachRow<FGuideBookPageData>(GetName(),
				[&Rows](const FName& Key, const FGuideBookPageData& Value)
				{
					if (!Key.IsNone())
					{
						Rows.Add(MakeShared<FPulldownRow>(Key.ToString(),
							FText::FromName(Key), Value.Label));
					}
				});
		}
		
		return Rows;
	}
};