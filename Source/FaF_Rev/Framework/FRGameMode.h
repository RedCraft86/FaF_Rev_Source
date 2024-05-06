// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTGameMode.h"
#include "GameMusicData.h"
#include "FRGameMode.generated.h"

#define FRGamemode(Context) AGTGameModeBase::Get<AFRGameModeBase>(Context)

UCLASS(Abstract)
class FAF_REV_API AFRGameModeBase final : public AGTGameModeBase
{
	GENERATED_BODY()

public:

	AFRGameModeBase();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		UAudioComponent* WorldMusicAudio;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		UAudioComponent* ChaseMusicAudio;

	UPROPERTY(Transient)
		class APhotoModeActor* PhotoModeActor;

	UPROPERTY(Transient)
		class AInspectionActor* InspectionActor;

	UFUNCTION(BlueprintCallable, Category = "GameMusic")
		void SetGameMusic(const FGameMusicID InMusicID);

	UFUNCTION(BlueprintCallable, Category = "GameMusic")
		void SetChaseMusic(const bool bIsChase);
	
	UFUNCTION(BlueprintCallable, Category = "GameMusic")
		void MuteGameMusic() const;

private:
	
	float FadeTime;
	bool bChaseMode;
	FTimerHandle FadeTimer;
	UPROPERTY(Transient) FGameMusicData MusicData;

	virtual void BeginPlay() override;
};
