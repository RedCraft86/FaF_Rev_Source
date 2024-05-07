// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "FaF_Rev.h"
#include "EnemyData.h"
#include "Engine/DataAsset.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "GameMusicData.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct FAF_REV_API FGameMusicTypeData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "GameMusic")
		TSoftObjectPtr<USoundBase> Music;

	UPROPERTY(EditAnywhere, Category = "GameMusic")
		bool bRestartOnTransition;

	FGameMusicTypeData() : Music(nullptr), bRestartOnTransition(false) {}
};

USTRUCT(BlueprintInternalUseOnly)
struct FAF_REV_API FGameMusicData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "GameMusic", meta = (ReadOnlyKeys))
		TMap<EEnemyAIMode, FGameMusicTypeData> EnemyModeToTracks;

#if WITH_EDITOR
protected:
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		Super::OnDataTableChanged(InDataTable, InRowName);
		
		if (EnemyModeToTracks.Num() == 4) return;
		for (const EEnemyAIMode Mode : TEnumRange<EEnemyAIMode>())
		{
			if (!EnemyModeToTracks.Contains(Mode)) EnemyModeToTracks.Add(Mode);
		}
	}
#endif
};

USTRUCT(BlueprintType)
struct FAF_REV_API FGameMusicID : public FPulldownStructBase
{
	GENERATED_BODY()
	SETUP_PULLDOWN(FGameMusicID)
	FGameMusicData GetMusicData() const;
};
