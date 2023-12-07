// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "AsyncActions/RCAsyncAwaitActorLeaveView.h"

URCAsyncAwaitActorLeaveView* URCAsyncAwaitActorLeaveView::AwaitActorLeaveView(const AActor* TargetActor, float MaxDistance, bool bOriginOnly, bool bLineTraceCheck, FActorBoundsCheckParams TraceCheckParams, float WaitTime, float CheckInterval)
{
	URCAsyncAwaitActorLeaveView* Task = NewObject<URCAsyncAwaitActorLeaveView>();
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

void URCAsyncAwaitActorLeaveView::Activate()
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
	World->GetTimerManager().SetTimer(TaskTimer, this, &URCAsyncAwaitActorLeaveView::TaskTick,
		_CheckInterval, true);
	
	World->GetTimerManager().SetTimer(WaitTimer, FTimerDelegate::CreateLambda([this]()
	{
		FinishTask(true);
	}), _WaitTime, false);
}

void URCAsyncAwaitActorLeaveView::TaskTick()
{
	if (!IsValid(_TargetActor))
	{
		FinishTask(true);
	}

	if (!URCRuntimeLibrary::IsActorInScreen(_TargetActor, _MaxDistance,
		_bOriginOnly, _bLineTraceCheck, _TraceCheckParams))
	{
		FinishTask(false);
	}
}

void URCAsyncAwaitActorLeaveView::FinishTask(bool bFailed)
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
		NeverLeftScreen.Broadcast();
	else
		LeftScreen.Broadcast();

	Active = false;
}