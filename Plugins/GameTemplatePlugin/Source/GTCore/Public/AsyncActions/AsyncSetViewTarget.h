// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "ExtendedAsyncAction.h"
#include "AsyncSetViewTarget.generated.h"

UCLASS()
class GTCORE_API UAsyncSetViewTarget final : public UExtendedAsyncAction
{
	GENERATED_BODY()

public:

	/* Set the view target blending with variable control.
	* @param NewTarget - New actor to set as view target.
	* @param BlendTime - Time taken to blend.
	* @param BlendFunc - Cubic, Linear etc functions for blending.
	* @param BlendExp -  Exponent, used by certain blend functions to control the shape of the curve.
	* @param bLockOutgoing - If true, lock outgoing view target to last frame's camera position for the remainder of the blend.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game|Player", meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true", DefaultToSelf = "NewTarget", AdvancedDisplay = "BlendFunc, BlendExp, bLockOutgoing", DisplayName = "Set View Target (Async)", Keywords = "Camera"))
		static UAsyncSetViewTarget* SetViewTargetAsync(UObject* WorldContextObject, AActor* NewTarget, const float BlendTime = 1.0f,
			const EViewTargetBlendFunction BlendFunc = VTBlend_Linear, const float BlendExp = 0.0f, const bool bLockOutgoing = false);
	
	UPROPERTY(BlueprintAssignable)
		FAsyncActionDelegate OnFinished;
	
private:

	virtual void Activate() override;
	void OnFinishChanging();

	UPROPERTY(Transient) TObjectPtr<AActor> _NewViewTarget = nullptr;
	EViewTargetBlendFunction _BlendFunc = VTBlend_Linear;
	bool _bLockOutgoing = false;
	float _BlendTime = 1.0f;
	float _BlendExp = 0.0f;
};
