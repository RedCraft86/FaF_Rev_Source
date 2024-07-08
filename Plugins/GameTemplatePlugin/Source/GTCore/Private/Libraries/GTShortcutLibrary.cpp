// Copyright (C) RedCraft86. All Rights Reserved.

#include "Libraries/GTShortcutLibrary.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "Sound/AmbientSound.h"

void UGTShortcutLibrary::ClearCameraFade(const UObject* ContextObject)
{
	if (APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(ContextObject, 0))
	{
		PCM->StopCameraFade();
	}
}

void UGTShortcutLibrary::SetCameraFade(const UObject* ContextObject, const float Alpha, const FLinearColor Color, const bool bFadeAudio)
{
	if (APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(ContextObject, 0))
	{
		PCM->SetManualCameraFade(Alpha, Color, bFadeAudio);
	}
}

void UGTShortcutLibrary::StartCameraFade(UObject* ContextObject, const float FromAlpha, const float ToAlpha,
	const float Duration, const FLinearColor Color, const bool bShouldFadeAudio, const bool bHoldWhenFinished)
{
	if (APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(ContextObject, 0))
	{
		PCM->StartCameraFade(FromAlpha, ToAlpha, Duration, Color, bShouldFadeAudio, bHoldWhenFinished);
	}
}

void UGTShortcutLibrary::SetViewTarget(UObject* ContextObject, AActor* NewTarget, const float BlendTime,
	const EViewTargetBlendFunction BlendFunc, const float BlendExp, const bool bLockOutgoing)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(ContextObject, 0))
	{
		PC->SetViewTargetWithBlend(NewTarget, BlendTime, BlendFunc, BlendExp, bLockOutgoing);
	}
}

void UGTShortcutLibrary::AddActorTag(AActor* Target, const FName InTag)
{
	if (IsValid(Target)) Target->Tags.AddUnique(InTag);
}

void UGTShortcutLibrary::RemoveActorTag(AActor* Target, const FName InTag)
{
	if (IsValid(Target)) Target->Tags.Remove(InTag);
}

void UGTShortcutLibrary::PlayLevelSequence(const ALevelSequenceActor* Target)
{
	if (ULevelSequencePlayer* Player = IsValid(Target) ? Target->GetSequencePlayer() : nullptr)
	{
		Player->Play();
	}
}

void UGTShortcutLibrary::StopLevelSequence(const ALevelSequenceActor* Target)
{
	if (ULevelSequencePlayer* Player = IsValid(Target) ? Target->GetSequencePlayer() : nullptr)
	{
		Player->Stop();
	}
}

void UGTShortcutLibrary::SkipLevelSequence(const ALevelSequenceActor* Target)
{
	if (ULevelSequencePlayer* Player = IsValid(Target) ? Target->GetSequencePlayer() : nullptr)
	{
		Player->GoToEndAndStop();
	}
}

void UGTShortcutLibrary::BindOnSequenceFinished(const ALevelSequenceActor* Target, const FRCBasicDelegate& Callback)
{
	ULevelSequencePlayer* Player = IsValid(Target) ? Target->GetSequencePlayer() : nullptr;
	if (Player && Callback.IsBound())
	{
		Player->OnFinished.AddUnique(Callback);
	}
}

void UGTShortcutLibrary::UnbindOnSequenceFinished(const ALevelSequenceActor* Target, const FRCBasicDelegate& Callback)
{
	ULevelSequencePlayer* Player = IsValid(Target) ? Target->GetSequencePlayer() : nullptr;
	if (Player && Callback.IsBound())
	{
		Player->OnFinished.Remove(Callback);
	}
}

void UGTShortcutLibrary::PlayAmbientSound(const AAmbientSound* Target, const float StartTime)
{
	if (UAudioComponent* Audio = IsValid(Target) ? Target->GetAudioComponent() : nullptr)
	{
		Audio->Play(StartTime);
	}
}

void UGTShortcutLibrary::StopAmbientSound(const AAmbientSound* Target, const float DelayTime)
{
	if (UAudioComponent* Audio = IsValid(Target) ? Target->GetAudioComponent() : nullptr)
	{
		Audio->StopDelayed(DelayTime);
	}
}

void UGTShortcutLibrary::FadeInAmbientSound(const AAmbientSound* Target, const float FadeTime, const float FadeVolume, const float StartTime)
{
	if (UAudioComponent* Audio = IsValid(Target) ? Target->GetAudioComponent() : nullptr)
	{
		Audio->FadeIn(FadeTime, FadeVolume, StartTime);
	}
}

void UGTShortcutLibrary::FadeOutAmbientSound(const AAmbientSound* Target, const float FadeTime, const float FadeVolume)
{
	if (UAudioComponent* Audio = IsValid(Target) ? Target->GetAudioComponent() : nullptr)
	{
		Audio->FadeOut(FadeTime, FadeVolume);
	}
}

void UGTShortcutLibrary::AdjustAmbientSoundVolume(const AAmbientSound* Target, const float FadeTime, const float FadeVolume)
{
	if (UAudioComponent* Audio = IsValid(Target) ? Target->GetAudioComponent() : nullptr)
	{
		Audio->AdjustVolume(FadeTime, FadeVolume);
	}
}

UActorComponent* UGTShortcutLibrary::AddActorInstanceComponent(AActor* Target, const TSubclassOf<UActorComponent> InClass)
{
#if WITH_EDITOR
if (Target && InClass) 
{
	UActorComponent* NewComponent = NewObject<UActorComponent>(Target, InClass);
	NewComponent->RegisterComponent();
	Target->AddInstanceComponent(NewComponent);
	return NewComponent;
}
#endif

	return nullptr;
}
