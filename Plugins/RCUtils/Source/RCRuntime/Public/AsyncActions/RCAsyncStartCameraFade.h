// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "RCAsyncActionBase.h"
#include "RCAsyncStartCameraFade.generated.h"

UCLASS()
class RCRUNTIME_API URCAsyncStartCameraFade final : public URCAsyncActionBase
{
	GENERATED_BODY()

public:

	/* Does a camera fade to/from a solid color. Animates automatically.
	* @param FromAlpha - Alpha at which to begin the fade. Range [0..1], where 0 is fully transparent and 1 is fully opaque solid color.
	* @param ToAlpha - Alpha at which to finish the fade.
	* @param Duration - How long the fade should take, in seconds.
	* @param Color - Color to fade to/from.
	* @param bShouldFadeAudio - True to fade audio volume along with the alpha of the solid color.
	* @param bHoldWhenFinished - True for fade to hold at the ToAlpha until explicitly stopped (e.g. with StopCameraFade).
	*/
	UFUNCTION(BlueprintCallable, Category = "AsyncActions", meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true", AdvancedDisplay = "Color, bHoldWhenFinished", DisplayName = "Start Camera Fade (Async)", Keywords = "Fade Camera"))
		static URCAsyncStartCameraFade* StartCameraFadeAsync(UObject* WorldContextObject, const float FromAlpha, const float ToAlpha = 1.0f, const float Duration = 1.0f, const FLinearColor Color = FLinearColor::Black, const bool bShouldFadeAudio = true, const bool bHoldWhenFinished = true);

	UPROPERTY(BlueprintAssignable)
		FRCAsyncActionDelegate OnCompleted;

	virtual void Activate() override;

private:

	float _FromAlpha = 0.0f;
	float _ToAlpha = 1.0f;
	float _Duration = 1.0f;
	FLinearColor _Color = FLinearColor::Black;
	bool _bShouldFadeAudio = true;
	bool _bHoldWhenFinished = true;

public: // Static functions (shortcuts)

	/* Stops camera fading and clears the currently faded state. */
	UFUNCTION(BlueprintCallable, Category = "Camera Fades", meta = (WorldContext = "WorldContextObject", Keywords = "Fade Camera"))
		static void ClearCameraFade(const UObject* WorldContextObject);

	/* Turns on camera fading at the given opacity. Does not auto-animate, allowing user to animate themselves.
	* @param Alpha - Alpha/Amount to set the fade to.
	* @param Color - Color of the fade.
	* @param bFadeAudio - True to fade audio volume along with the alpha of the solid color.
	*/
	UFUNCTION(BlueprintCallable, Category = "Camera Fades", meta = (WorldContext = "WorldContextObject", Keywords = "Fade Camera"))
		static void SetCameraFade(const UObject* WorldContextObject, const float Alpha, const FLinearColor Color, const bool bFadeAudio);

	/* Turns on camera fading at the given opacity. Does not auto-animate, allowing user to animate themselves.
	* @param Alpha - Alpha/Amount to set the fade to.
	* @param Color - Color of the fade.
	* @param bFadeAudio - True to fade audio volume along with the alpha of the solid color.
	*/
	UFUNCTION(BlueprintCallable, Category = "Camera Fades", meta = (WorldContext = "WorldContextObject", Keywords = "Fade Camera", AdvancedDisplay = "Color, bHoldWhenFinished"))
		static void StartCameraFade(UObject* WorldContextObject, const float FromAlpha, const float ToAlpha = 1.0f, const float Duration = 1.0f, const FLinearColor Color = FLinearColor::Black, const bool bShouldFadeAudio = true, const bool bHoldWhenFinished = true);
};
