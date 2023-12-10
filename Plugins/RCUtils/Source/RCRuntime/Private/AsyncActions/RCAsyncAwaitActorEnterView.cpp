// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "AsyncActions/RCAsyncAwaitActorEnterView.h"

URCAsyncAwaitActorEnterView* URCAsyncAwaitActorEnterView::AwaitActorEnterView(const AActor* TargetActor, float MaxDistance, bool bOriginOnly, bool bLineTraceCheck, FActorBoundsCheckParams TraceCheckParams, float WaitTime, float CheckInterval)
{
	URCAsyncAwaitActorEnterView* Task = NewObject<URCAsyncAwaitActorEnterView>();
	Task->WorldContext = const_cast<AActor*>(TargetActor);
	Task->_TargetActor = TargetActor;
	Task->_MaxDistance = MaxDistance;
	Task->_bOriginOnly = bOriginOnly;
	Task->_bLineTraceCheck = bLineTraceCheck;
	Task->_TraceCheckParams = TraceCheckParams;
	Task->_WaitTime = WaitTime;
	Task->_CheckInterval = CheckInterval;
	return Task;
}

void URCAsyncAwaitActorEnterView::Activate()
{
	if (Active)
	{
		FFrame::KismetExecutionMessage(TEXT("Task is active"), ELogVerbosity::Log);
		return;
	}

	if (!_TargetActor)
	{
		FFrame::KismetExecutionMessage(TEXT("Invalid Target Actor"), ELogVerbosity::Warning);
		return;
	}

	if (_MaxDistance <= 0.0f)
	{
		FFrame::KismetExecutionMessage(TEXT("Invalid Distance"), ELogVerbosity::Warning);
		return;
	}

	if (_TraceCheckParams.BoundingBoxLerp.Size() <= 0.0f)
	{
		FFrame::KismetExecutionMessage(TEXT("Invalid Box Lerp"), ELogVerbosity::Warning);
		return;
	}

	if (!_WaitTime)
	{
		FFrame::KismetExecutionMessage(TEXT("Invalid Wait Time"), ELogVerbosity::Warning);
		return;
	}

	if (!_CheckInterval)
	{
		FFrame::KismetExecutionMessage(TEXT("Invalid Check Interval"), ELogVerbosity::Warning);
		return;
	}

	Active = true;
	const UWorld* World = GetWorld();
	World->GetTimerManager().SetTimer(TaskTimer, this, &URCAsyncAwaitActorEnterView::TaskTick,
		_CheckInterval, true);
	
	World->GetTimerManager().SetTimer(WaitTimer, FTimerDelegate::CreateLambda([this]()
	{
		FinishTask(true);
	}), _WaitTime, false);
}

void URCAsyncAwaitActorEnterView::TaskTick()
{
	if (!IsValid(_TargetActor))
	{
		FinishTask(true);
	}

	if (URCRuntimeLibrary::IsActorInScreen(_TargetActor, _MaxDistance,
		_bOriginOnly, _bLineTraceCheck, _TraceCheckParams))
	{
		FinishTask(false);
	}
}

void URCAsyncAwaitActorEnterView::FinishTask(bool bFailed)
{
	if (!Active) return;
	
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(WaitTimer);
		WaitTimer.Invalidate();

		World->GetTimerManager().ClearTimer(TaskTimer);
		TaskTimer.Invalidate();
	}

	if (bFailed)
		NeverEnteredScreen.Broadcast();
	else
		EnteredScreen.Broadcast();

	Active = false;
}
