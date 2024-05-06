// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "FaF_Rev.h"
#include "Engine/DataAsset.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "GameMusicData.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct FAF_REV_API FGameMusicData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "GameMusic")
		TSoftObjectPtr<USoundBase> WorldMusic;

	UPROPERTY(EditAnywhere, Category = "GameMusic")
		TSoftObjectPtr<USoundBase> ChaseMusic;

	bool HasValidData() const { return !WorldMusic.IsNull() && !ChaseMusic.IsNull(); }
};

USTRUCT(BlueprintType)
struct FAF_REV_API FGameMusicID : public FPulldownStructBase
{
	GENERATED_BODY()
	SETUP_PULLDOWN(FGameMusicID)
	FGameMusicData GetMusicData() const;
};
