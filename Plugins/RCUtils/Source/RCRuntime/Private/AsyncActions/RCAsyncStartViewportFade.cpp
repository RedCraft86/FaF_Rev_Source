// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "AsyncActions/RCAsyncStartViewportFade.h"
#include "Rendering/RCGameViewportClient.h"

URCAsyncStartViewportFade* URCAsyncStartViewportFade::StartViewportFadeAsync(UObject* WorldContextObject, const float Target, const float Duration, const bool bFadeAudio)
{
	URCAsyncStartViewportFade* Task = NewObject<URCAsyncStartViewportFade>();
	Task->WorldContext = WorldContextObject;
	Task->_Target = Target;
	Task->_Duration = Duration;
	Task->_bFadeAudio = bFadeAudio;
	return Task;
}

void URCAsyncStartViewportFade::Activate()
{
	if (!IsValidWorldContext())
		return;

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		FFrame::KismetExecutionMessage(TEXT("World is null!"), ELogVerbosity::Warning);
		return;
	}

	URCGameViewportClient* VC = Cast<URCGameViewportClient>(World->GetGameViewport());
	if (!VC)
	{
		FFrame::KismetExecutionMessage(TEXT("Failed to get game viewport client."), ELogVerbosity::Warning);
        
		return;
	}

	VC->StartViewportFade(_Target, _Duration, _bFadeAudio);

	FTimerHandle Handle;
	World->GetTimerManager().SetTimer(Handle, [this](){
		OnCompleted.Broadcast();
	}, FMath::Max(_Duration, FApp::GetDeltaTime()), false);
}

void URCAsyncStartViewportFade::ClearViewportFade(const UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (URCGameViewportClient* VC = Cast<URCGameViewportClient>(World->GetGameViewport()))
		{
			VC->StopViewportFade();
		}
	}
}

void URCAsyncStartViewportFade::SetViewportFade(const UObject* WorldContextObject, const float Target, const bool bFadeAudio)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (URCGameViewportClient* VC = Cast<URCGameViewportClient>(World->GetGameViewport()))
		{
			VC->SetManualViewportFade(Target, bFadeAudio);
		}
	}
}
