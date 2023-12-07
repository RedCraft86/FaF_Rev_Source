// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "RCRuntimeLibrary.h"
#include "RCAsyncActionBase.h"
#include "RCAsyncAwaitActorLeaveView.generated.h"

UCLASS()
class RCRUNTIME_API URCAsyncAwaitActorLeaveView final : public URCAsyncActionBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
		FRCAsyncActionDelegate LeftScreen;

	UPROPERTY(BlueprintAssignable)
		FRCAsyncActionDelegate NeverLeftScreen;

	/* Waits the specified amount of time and checks if the specified actor ever leaves player view.
	 @param TargetActor - Actor to check
	 @param MaxDistance - The maximum distance before the actor is considered no longer in sight
	 @param bOriginOnly - If enabled, the checks will only make use of the actor's origin, bounds will not be used
	 @param bLineTraceCheck - Draw a line trace from the camera to the target when checking if it's in player view
	 @param TraceCheckParams - Settings to use when testing the different vertices of the actor's bounding box
	 @param WaitTime - Time to wait before cancelling the task
	 @param CheckInterval - The intervals in which the checks will take place
	*/
	UFUNCTION(BlueprintCallable, Category = "AsyncActions", meta = (BlueprintInternalUseOnly = "true", DefaultToSelf = "TargetActor", DisplayName = "Await Actor Leave Player View", Keywords = "actor leave player screen view"))
		static URCAsyncAwaitActorLeaveView* AwaitActorLeaveView(const AActor* TargetActor, float MaxDistance = 5000.0f, bool bOriginOnly = false, bool bLineTraceCheck = true, FActorBoundsCheckParams TraceCheckParams = FActorBoundsCheckParams(), float WaitTime = 15.0f, float CheckInterval = 0.15f);

	virtual void Activate() override;

private:

	UFUNCTION()
		void TaskTick();

	UFUNCTION()
		void FinishTask(bool bFailed);

	bool Active;
	FTimerHandle TaskTimer;
	FTimerHandle WaitTimer;

	UPROPERTY(Transient)
		const AActor* _TargetActor = nullptr;
	
	float _MaxDistance = 5000.0f;
	bool _bOriginOnly = false;
	bool _bLineTraceCheck = true;
	FActorBoundsCheckParams _TraceCheckParams = FActorBoundsCheckParams();
	float _WaitTime = 15.0f;
	float _CheckInterval = 0.25f;
};
