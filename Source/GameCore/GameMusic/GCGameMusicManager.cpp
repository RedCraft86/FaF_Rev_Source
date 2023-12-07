// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "GCGameMusicManager.h"
#include "Components/AudioComponent.h"
#include "Core/GCSettings.h"
#include "AudioDevice.h"

UGCGameMusicManager::UGCGameMusicManager()
{
	MainMusicComponent = nullptr;
	ChaseMusicComponent = nullptr;
	MainMusicSound = nullptr;
	ChaseMusicSound = nullptr;
	GameSettings = nullptr;
	bUseChaseMusic = false;
}

UGCGameMusicManager* UGCGameMusicManager::Get(const UObject* WorldContext)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	return World ? World->GetSubsystem<UGCGameMusicManager>() : nullptr;
}

void UGCGameMusicManager::SetMainMusic(USoundBase* InSound)
{
	if (!InSound)
		return;

	MainMusicSound = InSound;
	auto InitSound = [WEAK_THIS]()
	{
		if (!WeakThis.IsValid()) return;
		if (IsValid(WeakThis->MainMusicComponent) && WeakThis->MainMusicSound)
		{
			WeakThis->MainMusicComponent->SetSound(WeakThis->MainMusicSound);
			if (WeakThis->bUseChaseMusic)
			{
				WeakThis->MainMusicComponent->Play();
				WeakThis->MainMusicComponent->SetPaused(true);
			}
			else
			{
				WeakThis->MainMusicComponent->SetPaused(false);
				WeakThis->MainMusicComponent->FadeIn(WeakThis->GameSettings->MusicFadeTime, 1.0f);
			}
		}
	};
	
	if (MainMusicComponent)
	{
		if (MainMusicComponent->GetPlayState() != EAudioComponentPlayState::Stopped
			&& MainMusicComponent->GetPlayState() != EAudioComponentPlayState::Paused)
		{
			MainMusicComponent->FadeOut(GameSettings->MusicFadeTime, 0.0f);
		}
		
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, [InitSound](){
			InitSound();
		}, GameSettings->MusicFadeTime + 0.5f, false);
	}
	else
	{
		MainMusicComponent = CreateSound(MainMusicSound);
		InitSound();
	}
}

void UGCGameMusicManager::SetMainMusicPaused(const bool bPaused) const
{
	bPaused ? PauseMusicInternal(MainMusicComponent) : UnpauseMusicInternal(MainMusicComponent);
}

void UGCGameMusicManager::SetChaseMusic(USoundBase* InSound)
{
	if (!InSound)
		return;

	ChaseMusicSound = InSound;
	auto InitSound = [WEAK_THIS]()
	{
		if (!WeakThis.IsValid()) return;
		if (IsValid(WeakThis->ChaseMusicComponent) && WeakThis->ChaseMusicSound)
		{
			WeakThis->ChaseMusicComponent->SetSound(WeakThis->ChaseMusicSound);
			if (!WeakThis->bUseChaseMusic)
			{
				WeakThis->ChaseMusicComponent->Play();
				WeakThis->ChaseMusicComponent->SetPaused(true);
			}
			else
			{
				WeakThis->ChaseMusicComponent->SetPaused(false);
				WeakThis->ChaseMusicComponent->FadeIn(WeakThis->GameSettings->MusicFadeTime, 1.0f);
			}
		}
	};
	
	if (ChaseMusicComponent)
	{
		if (ChaseMusicComponent->GetPlayState() != EAudioComponentPlayState::Stopped
			&& ChaseMusicComponent->GetPlayState() != EAudioComponentPlayState::Paused)
		{
			ChaseMusicComponent->FadeOut(GameSettings->MusicFadeTime, 0.0f);
		}
		
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, [InitSound](){
			InitSound();
		}, GameSettings->MusicFadeTime + 0.5f, false);
	}
	else
	{
		ChaseMusicComponent = CreateSound(ChaseMusicSound);
		InitSound();
	}
}

void UGCGameMusicManager::SetChaseMusicPaused(const bool bPaused) const
{
	bPaused ? PauseMusicInternal(ChaseMusicComponent) : UnpauseMusicInternal(ChaseMusicComponent);
}

void UGCGameMusicManager::SetChaseMode(const bool bIsInChase)
{
	if (bUseChaseMusic != bIsInChase)
	{
		bUseChaseMusic = bIsInChase;
		SetMainMusicPaused(bUseChaseMusic);
		SetChaseMusicPaused(!bUseChaseMusic);
	}
}

void UGCGameMusicManager::StopAll()
{
	bUseChaseMusic = false;
	SetMainMusicPaused(true);
	SetChaseMusicPaused(true);
}

void UGCGameMusicManager::OnWorldBeginPlay(UWorld& World)
{
	Super::OnWorldBeginPlay(World);

	GameSettings = UGCSettings::Get();
	SetMainMusic(GameSettings->DefaultGameMusic.LoadSynchronous());
	SetChaseMusic(GameSettings->DefaultChaseMusic.LoadSynchronous());
}

bool UGCGameMusicManager::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE ||
		WorldType == EWorldType::GamePreview || WorldType == EWorldType::GameRPC;
}

UAudioComponent* UGCGameMusicManager::CreateSound(USoundBase* InSound) const
{
	const FAudioDevice::FCreateComponentParams Params(GetWorld()->GetAudioDeviceRaw());
	if (UAudioComponent* AudioComponent = FAudioDevice::CreateComponent(InSound, Params))
	{
		AudioComponent->SetVolumeMultiplier(1.0f);
		AudioComponent->SetPitchMultiplier(1.0f);
		AudioComponent->bCanPlayMultipleInstances = false;
		AudioComponent->bAllowSpatialization = false;
		AudioComponent->bIsUISound = true;
		AudioComponent->bAutoDestroy = false;
		AudioComponent->bIgnoreForFlushing = true;
		AudioComponent->SubtitlePriority = InSound->GetSubtitlePriority();
		AudioComponent->bStopWhenOwnerDestroyed = false;

		return AudioComponent;
	}

	return nullptr;
}

void UGCGameMusicManager::PauseMusicInternal(UAudioComponent* Component) const
{
	if (Component && !Component->bIsPaused && Component->GetPlayState() != EAudioComponentPlayState::FadingOut)
	{
		Component->AdjustVolume(GameSettings->MusicFadeTime, 0.1f);
		FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Component](float)->bool{
			Component->SetPaused(true);
			return false;
		}), GameSettings->MusicFadeTime + FApp::GetDeltaTime());
	}
}

void UGCGameMusicManager::UnpauseMusicInternal(UAudioComponent* Component) const
{
	if (Component && Component->bIsPaused)
	{
		Component->SetPaused(false);
		Component->AdjustVolume(0.0f, 0.1f);
		Component->AdjustVolume(GameSettings->MusicFadeTime, 1.0f);
	}
}