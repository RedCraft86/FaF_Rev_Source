// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "AsyncActions/RCAsyncSetViewTarget.h"
#include "GameFramework/PlayerController.h"

URCAsyncSetViewTarget* URCAsyncSetViewTarget::SetViewTargetAsync(UObject* WorldContextObject, AActor* NewTarget, const float BlendTime, const EViewTargetBlendFunction BlendFunc, const float BlendExp, const bool bLockOutgoing)
{
	URCAsyncSetViewTarget* Task = NewObject<URCAsyncSetViewTarget>();
	Task->WorldContext = WorldContextObject;
	Task->_NewViewTarget = NewTarget;
	Task->_BlendTime = BlendTime;
	Task->_BlendFunc = BlendFunc;
	Task->_BlendExp = BlendExp;
	Task->_bLockOutgoing = bLockOutgoing;
	return Task;
}

void URCAsyncSetViewTarget::Activate()
{
	if (!IsValidWorldContext()) { SetWorldContext(_NewViewTarget); }
	if (!IsValidWorldContext()) { return; }

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		FFrame::KismetExecutionMessage(TEXT("World is null!"), ELogVerbosity::Warning);
		return;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		FFrame::KismetExecutionMessage(TEXT("Failed to get player controller."), ELogVerbosity::Warning);
		return;
	}

	PC->SetViewTargetWithBlend(_NewViewTarget, _BlendTime, _BlendFunc, _BlendExp, _bLockOutgoing);
	
	FTimerHandle Handle;
	World->GetTimerManager().SetTimer(Handle, [this](){
		OnCompleted.Broadcast();
	}, FMath::Max(_BlendTime, FApp::GetDeltaTime()), false);
}

void URCAsyncSetViewTarget::SetViewTarget(UObject* WorldContextObject, AActor* NewTarget, const float BlendTime, const EViewTargetBlendFunction BlendFunc, const float BlendExp, const bool bLockOutgoing)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		World = GEngine->GetWorldFromContextObject(NewTarget, EGetWorldErrorMode::LogAndReturnNull);
	}

	if (APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr)
	{
		PC->SetViewTargetWithBlend(NewTarget, BlendTime, BlendFunc, BlendExp, bLockOutgoing);
	}
}
