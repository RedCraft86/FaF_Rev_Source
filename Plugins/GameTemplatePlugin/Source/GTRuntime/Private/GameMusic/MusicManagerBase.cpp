// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameMusic/MusicManagerBase.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GTSettings.h"
#include "GTGameMode.h"

void FMusicChannelComponents::NullCheck()
{
	TSet<const USoundBase*> NullComps;
	for (const TPair<TObjectPtr<const USoundBase>, TObjectPtr<UAudioComponent>>& Component : Components)
	{
		if (!Component.Value) NullComps.Add(Component.Key);
	}

	for (const USoundBase* Sound : NullComps)
	{
		Components.Remove(Sound);
	}
}

void FMusicChannelComponents::ClearComponents()
{
	for (const TPair<TObjectPtr<const USoundBase>, TObjectPtr<UAudioComponent>>& Component : Components)
	{
		if (Component.Value)
		{
			Component.Value->bAutoDestroy = true;
			Component.Value->Stop();
		}
	}
	Components.Empty();
}

TArray<TObjectPtr<UAudioComponent>> FMusicChannelComponents::GetComponents() const
{
	TArray<TObjectPtr<UAudioComponent>> Out;
	Components.GenerateValueArray(Out);
	return Out;
}

UAudioComponent* FMusicChannelComponents::FindComponent(const USoundBase* InSound)
{
	const TObjectPtr<UAudioComponent>* ComponentPtr = Components.Find(InSound);
	if (!ComponentPtr) return nullptr;
	
	if (TObjectPtr<UAudioComponent> Component = *ComponentPtr)
	{
		return Component;
	}

	Components.Remove(InSound);
	return nullptr;
}

void FMusicChannelComponents::AddComponent(UAudioComponent* InComponent)
{
	Components.Add(InComponent->Sound, InComponent);
}

AMusicManagerBase::AMusicManagerBase()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
	
#define CREATE_AUDIO_COMPONENT(Channel) \
	UAudioComponent* Channel = CreateDefaultSubobject<UAudioComponent>(#Channel TEXT("Audio")); \
	BaseAudioComponent.Add(EWorldMusicChannel::Channel, Channel);\
	Channel->SetupAttachment(GetRootComponent()); \
	Channel->bCanPlayMultipleInstances = false; \
	Channel->bAutoActivate = false; \
	Channel->SetUISound(true);
	
	CREATE_AUDIO_COMPONENT(Channel1);
	CREATE_AUDIO_COMPONENT(Channel2);
	CREATE_AUDIO_COMPONENT(Channel3);
	CREATE_AUDIO_COMPONENT(Channel4);
#undef CREATE_AUDIO_COMPONENT
}

void AMusicManagerBase::SetBaseMusicData(const UMusicDataBase* InData)
{
	if (InData && BaseMusicData != InData)
	{
		BaseMusicData = InData;
		for (TPair<EWorldMusicChannel, FMusicChannelComponents>& ExternalAudio : ExternalAudioComponents)
		{
			ExternalAudio.Value.ClearComponents();
		}
		ExternalAudioComponents.Empty();

		for (const EWorldMusicChannel Channel : TEnumRange<EWorldMusicChannel>())
		{
			ChannelStates.Add(Channel, false);
			if (UAudioComponent* BaseComp = BaseAudioComponent.FindRef(Channel))
			{
				BaseComp->bAutoDestroy = false;
				BaseComp->SetPaused(false);
				BaseComp->Stop();

				if (const FMusicChannelData* Data = BaseMusicData->Channels.Find(Channel))
				{
					BaseComp->SetSound(Data->Sound);
					BaseComp->SetVolumeMultiplier(Data->Volume);
					BaseComp->SetPitchMultiplier(Data->Pitch);
				}
			}
		}

		UnmuteChannel(EWorldMusicChannel::Channel1, true);
	}
}

UAudioComponent* AMusicManagerBase::FindExternalAudio(const EWorldMusicChannel InChannel, const USoundBase* InSound) const
{
	if (!InSound) return nullptr;
	return ExternalAudioComponents.FindRef(InChannel).FindComponent(InSound);
}

void AMusicManagerBase::AddExternalAudio(const EWorldMusicChannel InChannel, USoundBase* InSound, const float InVolume, const float InPitch, const float InStartTime, const bool bFade)
{
	if (!ChannelStates.FindRef(InChannel)) return;
	if (!InSound || FMath::IsNearlyZero(InVolume)|| FMath::IsNearlyZero(InPitch)) return;
	UAudioComponent* Component = FindExternalAudio(InChannel, InSound);
	if (!Component)
	{
		Component = UGameplayStatics::CreateSound2D(this, InSound, InVolume, InPitch, InStartTime);
	}

	Component->SetSound(InSound);
	Component->SetVolumeMultiplier(InVolume);
	Component->SetPitchMultiplier(InPitch);

	ExternalAudioComponents.FindOrAdd(InChannel).AddComponent(Component);
	if (bFade) Component->FadeIn(1.0f, 1.0f, InStartTime);
	else Component->Play(InStartTime);
}

void AMusicManagerBase::MuteChannel(const EWorldMusicChannel InChannel, const bool bImmediately)
{
	if (!ChannelStates.FindRef(InChannel)) return;
	
	ChannelStates.Add(InChannel, false);
	if (UAudioComponent* BaseComp = BaseAudioComponent.FindRef(InChannel))
	{
		if (bImmediately)
		{
			BaseComp->SetPaused(true);
		}
		else
		{
			float FadeTime = 1.0f;
			if (const FMusicChannelData* Data = BaseMusicData ? BaseMusicData->Channels.Find(InChannel) : nullptr)
			{
				FadeTime = Data->FadeTime;
			}
			
			BaseComp->AdjustVolume(FadeTime, 0.05f);

			if (GetWorldTimerManager().TimerExists(ChannelTimers.FindOrAdd(InChannel).Key))
			{
				GetWorldTimerManager().ClearTimer(ChannelTimers.FindOrAdd(InChannel).Key);
			}
			GetWorldTimerManager().SetTimer(ChannelTimers.FindOrAdd(InChannel).Key, [BaseComp]()
			{
				BaseComp->SetPaused(true);
			}, 1.0f, false);
		}
	}

	if (const FMusicChannelComponents* External = ExternalAudioComponents.Find(InChannel))
	{
		TArray<TObjectPtr<UAudioComponent>> Components = External->GetComponents();
		for (const TObjectPtr<UAudioComponent>& Component : Components)
		{
			if (bImmediately) Component->SetPaused(true);
			else Component->AdjustVolume(1.0f, 0.05f);
		}

		if (!bImmediately)
		{
			if (GetWorldTimerManager().TimerExists(ChannelTimers.FindOrAdd(InChannel).Value))
			{
				GetWorldTimerManager().ClearTimer(ChannelTimers.FindOrAdd(InChannel).Value);
			}
			GetWorldTimerManager().SetTimer(ChannelTimers.FindOrAdd(InChannel).Value, [Components]()
			{
				for (const TObjectPtr<UAudioComponent>& Component : Components)
				{
					if (Component) Component->SetPaused(true);
				}
			}, 1.0f, false);
		}
	}
}

void AMusicManagerBase::UnmuteChannel(const EWorldMusicChannel InChannel, const bool bImmediately)
{
	if (ChannelStates.FindRef(InChannel)) return;
	
	ChannelStates.Add(InChannel, true);
	if (UAudioComponent* BaseComp = BaseAudioComponent.FindRef(InChannel))
	{
		BaseComp->SetPaused(false);

		float FadeTime = 1.0f;
		bool bRestart = !BaseComp->IsPlaying();
		if (const FMusicChannelData* Data = BaseMusicData ? BaseMusicData->Channels.Find(InChannel) : nullptr)
		{
			FadeTime = Data->FadeTime;
			bRestart = bRestart || Data->bRestartOnMute;
		}

		if (bRestart)
		{
			BaseComp->FadeIn(bImmediately ? 0.0f : FadeTime, 1.0f);
		}
		else
		{
			BaseComp->AdjustVolume(bImmediately ? 0.0f : FadeTime, 1.0f);
		}
	}

	if (const FMusicChannelComponents* External = ExternalAudioComponents.Find(InChannel))
	{
		TArray<TObjectPtr<UAudioComponent>> Components = External->GetComponents();
		for (const TObjectPtr<UAudioComponent>& Component : Components)
		{
			Component->SetPaused(false);
			Component->AdjustVolume(bImmediately ? 0.0f : 1.0f, 1.0f);
		}
	}
}

void AMusicManagerBase::MuteChannels(const TSet<EWorldMusicChannel> InChannels, const bool bImmediately)
{
	for (const EWorldMusicChannel Channel : InChannels)
	{
		MuteChannel(Channel, bImmediately);
	}
}

void AMusicManagerBase::MuteAllChannels(const bool bImmediately)
{
	for (const EWorldMusicChannel Channel : TEnumRange<EWorldMusicChannel>())
	{
		MuteChannel(Channel, bImmediately);
	}
}

void AMusicManagerBase::NullCheck()
{
	for (TPair<EWorldMusicChannel, FMusicChannelComponents>& ExternalAudio : ExternalAudioComponents)
	{
		ExternalAudio.Value.NullCheck();
	}
}

void AMusicManagerBase::BeginPlay()
{
	Super::BeginPlay();
	for (const EWorldMusicChannel Channel : TEnumRange<EWorldMusicChannel>())
	{
		ChannelStates.Add(Channel, false);
	}
	GetWorldTimerManager().SetTimerForNextTick([this]()
	{
		SetBaseMusicData(UGTSettings::GetConst()->WorldMusic.LoadSynchronous());
	});

	GetWorldTimerManager().SetTimer(NullCheckTimer, this,
		&AMusicManagerBase::NullCheck, 1.0f, true);
}

AMusicManagerBase* AMusicManagerBase::GetMusicManager(const UObject* WorldContextObject)
{
	if (AGTGameModeBase* GameMode = AGTGameModeBase::Get(WorldContextObject))
	{
		return GameMode->GetMusicManager();
	}

	return nullptr;
}
