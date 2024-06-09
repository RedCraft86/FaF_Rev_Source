// Copyright (C) RedCraft86. All Rights Reserved.

#include "AsyncActions/AsyncSetCameraFade.h"
#include "Kismet/GameplayStatics.h"

UAsyncStartCameraFade* UAsyncStartCameraFade::StartCameraFadeAsync(UObject* WorldContextObject, const float FromAlpha,
	const float ToAlpha, const float Duration, const FLinearColor Color, const bool bShouldFadeAudio, const bool bHoldWhenFinished)
{
	CREATE_TASK
	Task->SetWorldContext(WorldContextObject);
	Task->_FromAlpha = FromAlpha;
	Task->_ToAlpha = ToAlpha;
	Task->_Duration = Duration;
	Task->_Color = Color;
	Task->_ShouldFadeAudio = bShouldFadeAudio;
	Task->_HoldWhenFinished = bHoldWhenFinished;
	return Task;
}

void UAsyncStartCameraFade::Activate()
{
	CHECK_RUNNING
	GET_WORLD

	APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(World, 0);
	if (!IsValid(PCM))
	{
		UE_KLOG_WARNING(2.0f, TEXT("Cannot get Camera Manager on Async Action %s."), *GetClass()->GetName())
		FINISH_RUNNING
		return;
	}
	
	PCM->StartCameraFade(_FromAlpha, _ToAlpha, _Duration, _Color, _ShouldFadeAudio, _HoldWhenFinished);

	FTimerHandle Handle;
	World->GetTimerManager().SetTimer(Handle, this, &UAsyncStartCameraFade::OnFinishFading,
		_Duration + UE_KINDA_SMALL_NUMBER, false);
}

void UAsyncStartCameraFade::OnFinishFading()
{
	OnFinished.Broadcast();
	FINISH_RUNNING
}
