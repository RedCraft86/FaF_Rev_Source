// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "RCAsyncActionBase.h"
#include "Camera/PlayerCameraManager.h"
#include "RCAsyncSetViewTarget.generated.h"

UCLASS()
class RCRUNTIME_API URCAsyncSetViewTarget final : public URCAsyncActionBase
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
		static URCAsyncSetViewTarget* SetViewTargetAsync(UObject* WorldContextObject, AActor* NewTarget, const float BlendTime = 1.0f, const EViewTargetBlendFunction BlendFunc = VTBlend_Linear, const float BlendExp = 0.0f, const bool bLockOutgoing = false);

	UPROPERTY(BlueprintAssignable)
		FRCAsyncActionDelegate OnCompleted;

	virtual void Activate() override;

private:

	UPROPERTY(Transient)
		AActor* _NewViewTarget = nullptr;
	
	float _BlendTime = 1.0f;
	EViewTargetBlendFunction _BlendFunc = VTBlend_Linear;
	float _BlendExp = 0.0f;
	bool _bLockOutgoing = false;

public: // Static functions (shortcuts)

	/* Set the view target blending with variable control.
	* @param NewTarget - New actor to set as view target.
	* @param BlendTime - Time taken to blend.
	* @param BlendFunc - Cubic, Linear etc functions for blending.
	* @param BlendExp -  Exponent, used by certain blend functions to control the shape of the curve.
	* @param bLockOutgoing - If true, lock outgoing view target to last frame's camera position for the remainder of the blend.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game|Player", meta = (WorldContext = "WorldContextObject", DefaultToSelf = "NewTarget", AdvancedDisplay = "BlendFunc, BlendExp, bLockOutgoing", Keywords = "Camera"))
		static void SetViewTarget(UObject* WorldContextObject, AActor* NewTarget, const float BlendTime = 1.0f, const EViewTargetBlendFunction BlendFunc = VTBlend_Linear, const float BlendExp = 0.0f, const bool bLockOutgoing = false);
};
