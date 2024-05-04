// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "PlayerData.h"
#include "ListGeneratorBase.h"
#include "ListGen_PlayerLockFlags.generated.h"

UCLASS(DisplayName = "[] List Generator - Player Lock Flags")
class UListGen_PlayerLockFlags final : public USimpleListGenerator
{
	GENERATED_BODY()
	
	virtual TArray<TSharedPtr<FPulldownRow>> OnGenerate() const override
	{
		TArray<TSharedPtr<FPulldownRow>> Rows;
		for (const FName& Flag : Player::LockFlags::All)
		{
			Rows.Add(MakeShared<FPulldownRow>(Flag.ToString(), FText::FromName(Flag),
				FText::FromString(FName::NameToDisplayString(Flag.ToString(), false))));
		}
		return Rows;
	}
};