// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Data/MiscTypes.h"
#include "ExtendedAsyncAction.h"
#include "AsyncAwaitActorEnterView.generated.h"

UCLASS()
class GTCORE_API UAsyncAwaitActorEnterView final : public UExtendedAsyncAction
{
	GENERATED_BODY()
	
public:

	/* Waits for the actor to enter the player view.
	* @param Target - Actor to check.
	* @param MaxDistance - The maximum distance before the actor is considered no longer in sight.
	* @param bOriginOnly - If enabled, the checks will only make use of the actor's origin, bounds will not be used.
	* @param bLineTraceCheck - Draw a line trace from the camera to the target when checking if it's in player view.
	* @param TraceCheckParams - Settings to use when testing the different vertices of the actor's bounding box.
	* @param WaitTime - Time to wait before the task times out.
	* @param CheckInterval - Interval to check the actor state on.
	* @return Whether or not the actor is visible in screen with the given settings.
	*/
	UFUNCTION(BlueprintCallable, Category = "Actor", meta = (BlueprintInternalUseOnly = "true", DefaultToSelf = "Target", AutoCreateRefTerm = "TraceCheckParams", AdvancedDisplay = "TraceCheckParams, WaitTime, CheckInterval", DisplayName = "Await Actor Enter View (Async)", Keywords = "Await Wait Actor View Enter"))
		static UAsyncAwaitActorEnterView* AwaitActorEnterView(AActor* Target, const float MaxDistance = 5000.0f,
			const bool bOriginOnly = false, const bool bLineTraceCheck = true,
			const FActorBoundsCheckParams& TraceCheckParams = FActorBoundsCheckParams(),
			float WaitTime = 15.0f, float CheckInterval = 0.15f);

	UPROPERTY(BlueprintAssignable)
		FAsyncActionDelegate OnSucceeded;

	UPROPERTY(BlueprintAssignable)
		FAsyncActionDelegate OnTimedOut;
	
private:

	virtual void Activate() override;
	void OnTimerTick();
	void OnTimerEnded();

	UPROPERTY(Transient) TObjectPtr<AActor> _Target = nullptr;
	FActorBoundsCheckParams _TraceCheckParams = {};
	float _MaxDistance = 5000.0f;
	bool _OriginOnly = false;
	bool _LineTraceCheck = true;
	float _WaitTime = 15.0f;
	float _CheckInterval = 0.15f;
	
	FTimerHandle WaitHandle;
	FTimerHandle TickHandle;
	bool bSucceeded = false;
};
