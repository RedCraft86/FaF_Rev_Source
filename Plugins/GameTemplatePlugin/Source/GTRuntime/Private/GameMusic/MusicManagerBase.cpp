// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameMusic/MusicManagerBase.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

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
	if (!InSound || FMath::IsNearlyZero(InVolume)|| FMath::IsNearlyZero(InPitch)) return;
	UAudioComponent* OldComponent = FindExternalAudio(InChannel, InSound);
	if (!OldComponent)
	{
		OldComponent = UGameplayStatics::CreateSound2D(this, InSound, InVolume, InPitch, InStartTime);
	}

	ExternalAudioComponents.FindOrAdd(InChannel).AddComponent(OldComponent);
	if (bFade) OldComponent->FadeIn(1.0f, InStartTime);
	else OldComponent->Play(InStartTime);
}

void AMusicManagerBase::MuteChannel(const EWorldMusicChannel InChannel, const bool bImmediately)
{
	if (UAudioComponent* BaseComp = BaseAudioComponent.FindRef(InChannel))
	{
		if (bImmediately)
		{
			BaseComp->SetPaused(true);
		}
		else
		{
			float FadeTime = 0.5f;
			if (const FMusicChannelData* Data = BaseMusicData ? BaseMusicData->Channels.Find(InChannel) : nullptr)
			{
				FadeTime = Data->FadeTime;
			}
			
			BaseComp->AdjustVolume(FadeTime, 0.05f);
			GetWorldTimerManager().ClearTimer(ChannelTimers.FindOrAdd(InChannel).Key);
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
			GetWorldTimerManager().ClearTimer(ChannelTimers.FindOrAdd(InChannel).Value);
			GetWorldTimerManager().SetTimer(ChannelTimers.FindOrAdd(InChannel).Value, [&Components]()
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
	if (UAudioComponent* BaseComp = BaseAudioComponent.FindRef(InChannel))
	{
		BaseComp->SetPaused(false);

		float FadeTime = 0.0f;
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

void AMusicManagerBase::MuteAllChannels(const bool bImmediately)
{
	for (const EWorldMusicChannel Channel : TEnumRange<EWorldMusicChannel>())
	{
		MuteChannel(Channel, bImmediately);
	}
}
