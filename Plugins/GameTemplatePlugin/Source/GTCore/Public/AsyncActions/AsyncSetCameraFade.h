// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "ExtendedAsyncAction.h"
#include "AsyncSetCameraFade.generated.h"

UCLASS()
class GTCORE_API UAsyncStartCameraFade final : public UExtendedAsyncAction
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
	UFUNCTION(BlueprintCallable, Category = "Camera Fades", meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true", AdvancedDisplay = "Color, bHoldWhenFinished", DisplayName = "Start Camera Fade (Async)", Keywords = "Fade Camera"))
		static UAsyncStartCameraFade* StartCameraFadeAsync(UObject* WorldContextObject, const float FromAlpha = 0.0f,
			const float ToAlpha = 1.0f, const float Duration = 1.0f, const FLinearColor Color = FLinearColor::Black,
			const bool bShouldFadeAudio = true, const bool bHoldWhenFinished = true);

	UPROPERTY(BlueprintAssignable)
		FAsyncActionDelegate OnFinished;
	
private:

	virtual void Activate() override;
	void OnFinishFading();

	float _FromAlpha = 0.0f;
	float _ToAlpha = 1.0f;
	float _Duration = 1.0f;
	FLinearColor _Color = FLinearColor::Black;
	bool _ShouldFadeAudio = true;
	bool _HoldWhenFinished = true;
};
