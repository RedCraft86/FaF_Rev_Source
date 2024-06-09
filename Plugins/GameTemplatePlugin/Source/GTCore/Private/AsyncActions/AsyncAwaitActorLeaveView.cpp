// Copyright (C) RedCraft86. All Rights Reserved.

#include "AsyncActions/AsyncAwaitActorLeaveView.h"
#include "Libraries/GTRuntimeLibrary.h"

UAsyncAwaitActorLeaveView* UAsyncAwaitActorLeaveView::AwaitActorLeaveView(AActor* Target, const float MaxDistance,
	const bool bOriginOnly, const bool bLineTraceCheck, const FActorBoundsCheckParams& TraceCheckParams,
	const float WaitTime, const float CheckInterval)
{
	CREATE_TASK
	Task->SetWorldContext(Target);
	Task->_MaxDistance = MaxDistance;
	Task->_OriginOnly = bOriginOnly;
	Task->_LineTraceCheck = bLineTraceCheck;
	Task->_TraceCheckParams = TraceCheckParams;
	Task->_WaitTime = WaitTime;
	Task->_CheckInterval = CheckInterval;
	return Task;
}

void UAsyncAwaitActorLeaveView::Activate()
{
	CHECK_RUNNING
	GET_WORLD
	FTimerManager& TimerManager = World->GetTimerManager();
	TimerManager.SetTimer(WaitHandle, this, &UAsyncAwaitActorLeaveView::OnTimerEnded, _WaitTime, false);
	TimerManager.SetTimer(TickHandle, this, &UAsyncAwaitActorLeaveView::OnTimerTick,
		_CheckInterval, true, 0.0f);
}

void UAsyncAwaitActorLeaveView::OnTimerTick()
{
	if (bSucceeded) return;
	if (!UGTRuntimeLibrary::IsActorInScreen(_Target, _MaxDistance, _OriginOnly, _LineTraceCheck, _TraceCheckParams))
	{
		bSucceeded = true;
		OnTimerEnded();
	}
}

void UAsyncAwaitActorLeaveView::OnTimerEnded()
{
	GET_WORLD
	World->GetTimerManager().ClearTimer(WaitHandle);
	World->GetTimerManager().ClearTimer(TickHandle);
	if (bSucceeded)
	{
		OnSucceeded.Broadcast();
	}
	else
	{
		OnTimedOut.Broadcast();
	}

	FINISH_RUNNING
}
