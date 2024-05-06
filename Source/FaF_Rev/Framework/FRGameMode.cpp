// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRGameMode.h"
#include "Components/AudioComponent.h"
#include "FRPlayerController.h"
#include "FRSettings.h"
#include "FRPlayer.h"

AFRGameModeBase::AFRGameModeBase()
	: PhotoModeActor(nullptr), InspectionActor(nullptr)
{
	PlayerControllerClass = AFRPlayerController::StaticClass();
	DefaultPawnClass = AFRPlayerBase::StaticClass();

	WorldMusicAudio = CreateDefaultSubobject<UAudioComponent>("WorldMusicAudio");
	WorldMusicAudio->SetupAttachment(GetRootComponent());
	WorldMusicAudio->bCanPlayMultipleInstances = false;
	WorldMusicAudio->bAutoActivate = false;

	ChaseMusicAudio = CreateDefaultSubobject<UAudioComponent>("ChaseMusicAudio");
	ChaseMusicAudio->SetupAttachment(GetRootComponent());
	ChaseMusicAudio->bCanPlayMultipleInstances = false;
	ChaseMusicAudio->bAutoActivate = false;
}

void AFRGameModeBase::SetGameMusic(const FGameMusicID InMusicID)
{
	MuteGameMusic();
	bChaseMode = false;
	MusicData = InMusicID.GetMusicData();
	if (!MusicData.HasValidData())
	{
		SMART_LOG(Error, TEXT("Attempting to Set the Game Music with an invalid ID."))
		return;
	}

	WorldMusicAudio->SetSound(MusicData.WorldMusic.LoadSynchronous());
	WorldMusicAudio->FadeIn(FadeTime);

	ChaseMusicAudio->SetSound(MusicData.ChaseMusic.LoadSynchronous());
	ChaseMusicAudio->FadeIn(0.0f, 0.05f);
	ChaseMusicAudio->SetPaused(true);
}

void AFRGameModeBase::SetChaseMusic(const bool bIsChase)
{
	if (bChaseMode != bIsChase)
	{
		bChaseMode = bIsChase;
		if (bChaseMode)
		{
			ChaseMusicAudio->SetPaused(false);
			ChaseMusicAudio->AdjustVolume(FadeTime, 1.0f);
			WorldMusicAudio->AdjustVolume(FadeTime * 0.5f, 0.05f);
		}
		else
		{
			WorldMusicAudio->SetPaused(false);
			WorldMusicAudio->AdjustVolume(FadeTime, 1.0f);
			ChaseMusicAudio->AdjustVolume(FadeTime * 0.5f, 0.05f);
		}
	}

	GetWorldTimerManager().ClearTimer(FadeTimer);
	GetWorldTimerManager().SetTimer(FadeTimer, [this]()
	{
		WorldMusicAudio->SetPaused(bChaseMode);
		ChaseMusicAudio->SetPaused(!bChaseMode);
	}, FadeTime * 0.55f, false);
}

void AFRGameModeBase::MuteGameMusic() const
{
	WorldMusicAudio->SetPaused(false);
	WorldMusicAudio->Stop();

	ChaseMusicAudio->SetPaused(false);
	ChaseMusicAudio->Stop();
}

void AFRGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (const UFRSettings* Settings = FRSettings)
	{
		FadeTime = Settings->MusicFadeTime;
		if (!Settings->OnGameplayMap(this))
			SetGameMusic(Settings->DefaultMusicID);
	}
}
