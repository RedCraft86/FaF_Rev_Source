﻿// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "ExecEnums.h"
#include "GameMusicData.h"
#include "GameFramework/GameStateBase.h"
#include "FRGameState.generated.h"

#define FRGameState(Context) AFRGameState::Get<AFRGameState>(Context)

UCLASS()
class FAF_REV_API AFRGameStateBase final : public AGameStateBase
{
	GENERATED_BODY()

public:
	
	AFRGameStateBase();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		UAudioComponent* WorldMusicAudio;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		UAudioComponent* SuspicionMusicAudio;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		UAudioComponent* ChaseMusicAudio;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		UAudioComponent* SearchMusicAudio;

	UFUNCTION(BlueprintCallable, Category = "GameMusic")
		void SetGameMusic(const FGameMusicID InMusicID);

	UFUNCTION(BlueprintCallable, Category = "GameMusic")
		void SetMusicMode(const EEnemyAIMode InMusicMode);
	
	UFUNCTION(BlueprintCallable, Category = "GameMusic")
		void StopGameMusic() const;

protected:

	float FadeTime;
	FTimerHandle FadeTimer;
	EEnemyAIMode MusicMode;
	
	UPROPERTY(Transient)
		FGameMusicData MusicTracks;

	UPROPERTY(Transient)
		TMap<EEnemyAIMode, UAudioComponent*> EnemyModeToAudio;

	virtual void BeginPlay() override;

public: // Statics

	UFUNCTION(BlueprintPure, Category = "Game", DisplayName = "Get Game State (Smart)", meta = (DynamicOutputParam = "ReturnValue", DeterminesOutputType = "Class", WorldContext = "WorldContextObject"))
		static AFRGameStateBase* GetGameStateSmart(const UObject* WorldContextObject, const TSubclassOf<AFRGameStateBase> Class);
	
	UFUNCTION(BlueprintCallable, Category = "Game", DisplayName = "Get Game State (Checked)", meta = (DynamicOutputParam = "OutObject", DeterminesOutputType = "Class", ExpandEnumAsExecs = "ReturnValue", WorldContext = "WorldContextObject"))
		static EGTValidPins GetGameStateChecked(AFRGameStateBase*& OutObject, const UObject* WorldContextObject, const TSubclassOf<AFRGameStateBase> Class);
	
	template <typename T = AFRGameStateBase>
	static T* Get(const UObject* WorldContextObject)
	{
		return Cast<T>(GetGameStateSmart(WorldContextObject, T::StaticClass()));
	}
};