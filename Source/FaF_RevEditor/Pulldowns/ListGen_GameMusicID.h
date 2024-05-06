// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "FRSettings.h"
#include "GameMusicData.h"
#include "ListGeneratorBase.h"
#include "ListGen_GameMusicID.generated.h"

UCLASS(DisplayName = "[] List Generator - Game Music ID")
class UListGen_GameMusicID final : public UTableListGenerator
{
	GENERATED_BODY()

public:

	virtual FName GetDataObjectSettingName() const override { return GET_MEMBER_NAME_CHECKED(UFRSettings, MusicTable); }
	virtual TSoftObjectPtr<UObject> GetDataObject() const override { return FRSettings->MusicTable; }

private:
	
	virtual TArray<TSharedPtr<FPulldownRow>> OnGenerate() const override
	{
		TArray<TSharedPtr<FPulldownRow>> Rows;
		if (GetDataObj(const UDataTable, Table))
		{
			Table->ForeachRow<FGameMusicData>(GetName(),
				[&Rows](const FName& Key, const FGameMusicData& Value)
				{
					if (!Key.IsNone())
					{
						Rows.Add(MakeShared<FPulldownRow>(Key.ToString(), FText::FromName(Key),
							FText::FromString(FName::NameToDisplayString(Key.ToString(), false))));
					}
				});
		}
		
		return Rows;
	}
};