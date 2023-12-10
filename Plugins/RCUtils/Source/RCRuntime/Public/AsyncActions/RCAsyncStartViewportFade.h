// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "RCAsyncActionBase.h"
#include "RCAsyncStartViewportFade.generated.h"

UCLASS()
class RCRUNTIME_API URCAsyncStartViewportFade final : public URCAsyncActionBase
{
	GENERATED_BODY()

public:

	/* Does a viewport fade to/from a solid color. Animates automatically.
	* @param Target - Alpha at which to finish the fade.
	* @param Duration - How long the fade should take, in seconds.
	* @param bFadeAudio - True to fade audio volume along with the alpha of the solid color.
	*/
	UFUNCTION(BlueprintCallable, Category = "Viewport Fades", meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true", DisplayName = "Start Viewport Fade (Async)", Keywords = "Fade Viewport"))
		static URCAsyncStartViewportFade* StartViewportFadeAsync(UObject* WorldContextObject, const float Target = 1.0f, const float Duration = 1.0f, const bool bFadeAudio = true);

	UPROPERTY(BlueprintAssignable)
		FRCAsyncActionDelegate OnCompleted;

	virtual void Activate() override;

private:

	float _Target = 0.0f;
	float _Duration = 1.0f;
	bool _bFadeAudio = true;

public: // Static functions (shortcuts)

	/* Stops viewport fading and clears the currently faded state. */
	UFUNCTION(BlueprintCallable, Category = "Viewport Fades", meta = (WorldContext = "WorldContextObject", Keywords = "Fade Viewport"))
		static void ClearViewportFade(const UObject* WorldContextObject);

	/* Turns on viewport fading at the given opacity. Does not auto-animate, allowing user to animate themselves.
	* @param Target - Alpha/Amount to set the fade to.
	* @param bFadeAudio - True to fade audio volume along with the alpha of the solid color.
	*/
	UFUNCTION(BlueprintCallable, Category = "Viewport Fades", meta = (WorldContext = "WorldContextObject", Keywords = "Fade Viewport"))
		static void SetViewportFade(const UObject* WorldContextObject, const float Target, const bool bFadeAudio);
};