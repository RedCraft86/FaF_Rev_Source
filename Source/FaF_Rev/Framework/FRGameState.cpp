// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRGameState.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "FRSettings.h"

#define CREATE_AUDIO_COMPONENT(Name) \
	Name = CreateDefaultSubobject<UAudioComponent>(#Name); \
	Name->SetupAttachment(GetRootComponent()); \
	Name->bCanPlayMultipleInstances = false; \
	Name->bAutoActivate = false; \
	Name->SetUISound(true);

AFRGameStateBase::AFRGameStateBase()
{
	CREATE_AUDIO_COMPONENT(WorldMusicAudio);
	CREATE_AUDIO_COMPONENT(SuspicionMusicAudio);
	CREATE_AUDIO_COMPONENT(ChaseMusicAudio);
	CREATE_AUDIO_COMPONENT(SearchMusicAudio);

	EnemyModeToAudio = {
		{EEnemyAIMode::None, WorldMusicAudio},
		{EEnemyAIMode::Suspicion, SuspicionMusicAudio},
		{EEnemyAIMode::Chase, ChaseMusicAudio},
		{EEnemyAIMode::Search, SearchMusicAudio}
	};
}

void AFRGameStateBase::SetGameMusic(const FGameMusicID InMusicID)
{
	StopGameMusic();
	MusicMode = EEnemyAIMode::None;
	MusicTracks = InMusicID.GetMusicData();

	for (const EEnemyAIMode Mode : TEnumRange<EEnemyAIMode>())
	{
		UAudioComponent* Comp = EnemyModeToAudio.FindRef(Mode);
		if (!Comp) continue;

		const FGameMusicTypeData* Data = MusicTracks.EnemyModeToTracks.Find(Mode);
		if (Data && !Data->Music.IsNull()) continue;

		Comp->SetSound(Data->Music.LoadSynchronous());
		if (Mode == MusicMode)
		{
			Comp->FadeIn(FadeTime);
		}
		else
		{
			Comp->FadeIn(0.0f, 0.05f);
			Comp->SetPaused(true);
		}
	}
}

void AFRGameStateBase::SetMusicMode(const EEnemyAIMode InMusicMode)
{
	if (MusicMode == InMusicMode) return;

	MusicMode = InMusicMode;
	for (const EEnemyAIMode Mode : TEnumRange<EEnemyAIMode>())
	{
		UAudioComponent* Comp = EnemyModeToAudio.FindRef(Mode);
		if (!Comp || !Comp->GetSound()) continue;
			
		if (Mode == MusicMode)
		{
			Comp->SetPaused(false);
			if (MusicTracks.EnemyModeToTracks.FindRef(Mode).bRestartOnTransition)
			{
				Comp->FadeIn(FadeTime, 1.0f);
			}
			else
			{
				Comp->AdjustVolume(FadeTime, 1.0f);
			}
		}
		else
		{
			Comp->AdjustVolume(FadeTime * 0.5f, 0.05f);
		}
	}

	GetWorldTimerManager().ClearTimer(FadeTimer);
	GetWorldTimerManager().SetTimer(FadeTimer, [this]()
	{
		for (const EEnemyAIMode Mode : TEnumRange<EEnemyAIMode>())
		{
			if (UAudioComponent* Comp = EnemyModeToAudio.FindRef(Mode))
			{
				Comp->SetPaused(Mode != MusicMode);
			}
		}
	}, FadeTime * 0.55f, false);
}

void AFRGameStateBase::StopGameMusic() const
{
	for (const EEnemyAIMode Mode : TEnumRange<EEnemyAIMode>())
	{
		if (UAudioComponent* Comp = EnemyModeToAudio.FindRef(Mode))
		{
			Comp->SetPaused(false);
			Comp->Stop();
		}
	}
}

void AFRGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	if (const UFRSettings* Settings = FRSettings)
	{
		FadeTime = Settings->MusicFadeTime;
		if (!Settings->IsGameplayMap(this))
			SetGameMusic(Settings->DefaultMusicID);
	}
}

AFRGameStateBase* AFRGameStateBase::GetGameStateSmart(const UObject* WorldContextObject, const TSubclassOf<AFRGameStateBase> Class)
{
	AFRGameStateBase* Obj = Cast<AFRGameStateBase>(UGameplayStatics::GetGameState(WorldContextObject));
	return Obj && Obj->IsA(Class) ? Obj : nullptr;
}

EGTValidPins AFRGameStateBase::GetGameStateChecked(AFRGameStateBase*& OutObject, const UObject* WorldContextObject, const TSubclassOf<AFRGameStateBase> Class)
{
	OutObject = GetGameStateSmart(WorldContextObject, Class);
	return IsValid(OutObject) ? EGTValidPins::Valid : EGTValidPins::NotValid;
}
