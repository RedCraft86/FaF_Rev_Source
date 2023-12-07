// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "AsyncActions/RCAsyncStartCameraFade.h"
#include "GameFramework/PlayerController.h"

URCAsyncStartCameraFade* URCAsyncStartCameraFade::StartCameraFadeAsync(UObject* WorldContextObject, const float FromAlpha, const float ToAlpha, const float Duration, const FLinearColor Color, const bool bShouldFadeAudio, const bool bHoldWhenFinished)
{
    URCAsyncStartCameraFade* Task = NewObject<URCAsyncStartCameraFade>();
    Task->WorldContext = WorldContextObject;
    Task->_FromAlpha = FromAlpha;
    Task->_ToAlpha = ToAlpha;
    Task->_Duration = Duration;
    Task->_Color = Color;
    Task->_bShouldFadeAudio = bShouldFadeAudio;
    Task->_bHoldWhenFinished = bHoldWhenFinished;
    return Task;
}

void URCAsyncStartCameraFade::Activate()
{
    if (!IsValidWorldContext())
        return;

    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
        FFrame::KismetExecutionMessage(TEXT("World is null!"), ELogVerbosity::Warning);
        return;
    }

    const APlayerController* PC = World->GetFirstPlayerController();
    if (!PC || !PC->PlayerCameraManager)
    {
        FFrame::KismetExecutionMessage(!PC->PlayerCameraManager ?
            TEXT("Failed to get player camera manager.") : TEXT("Failed to get player controller."),
            ELogVerbosity::Warning);
        
        return;
    }

    PC->PlayerCameraManager->StartCameraFade(_FromAlpha, _ToAlpha, _Duration, _Color, _bShouldFadeAudio, _bHoldWhenFinished);

    FTimerHandle Handle;
    World->GetTimerManager().SetTimer(Handle, [this](){
        OnCompleted.Broadcast();
    }, FMath::Max(_Duration, FApp::GetDeltaTime()), false);
}

void URCAsyncStartCameraFade::ClearCameraFade(const UObject* WorldContextObject)
{
    if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        if (const APlayerController* PC = World->GetFirstPlayerController())
        {
            PC->PlayerCameraManager->StopCameraFade();
        }
    }
}

void URCAsyncStartCameraFade::SetCameraFade(const UObject* WorldContextObject, const float Alpha, const FLinearColor Color, const bool bFadeAudio)
{
    if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        if (const APlayerController* PC = World->GetFirstPlayerController())
        {
            PC->PlayerCameraManager->SetManualCameraFade(Alpha, Color, bFadeAudio);
        }
    }
}

void URCAsyncStartCameraFade::StartCameraFade(UObject* WorldContextObject, const float FromAlpha, const float ToAlpha, const float Duration, const FLinearColor Color, const bool bShouldFadeAudio, const bool bHoldWhenFinished)
{
    if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        if (const APlayerController* PC = World->GetFirstPlayerController())
        {
            PC->PlayerCameraManager->StartCameraFade(FromAlpha, ToAlpha, Duration, Color, bShouldFadeAudio, bHoldWhenFinished);
        }
    }
}
