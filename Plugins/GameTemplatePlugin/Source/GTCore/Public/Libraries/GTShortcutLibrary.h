// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GTShortcutLibrary.generated.h"

class ALevelSequenceActor;
class AAmbientSound;

DECLARE_DYNAMIC_DELEGATE(FRCBasicDelegate);

UCLASS(DisplayName = "Shortcut Library")
class GTCORE_API UGTShortcutLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	/* Stops camera fading and clears the currently faded state. */
	UFUNCTION(BlueprintCallable, Category = "Camera Fades", meta = (WorldContext = "ContextObject", Keywords = "Fade Camera"))
		static void ClearCameraFade(const UObject* ContextObject);

	/* Turns on camera fading at the given opacity. Does not auto-animate, allowing user to animate themselves.
	* @param Alpha - Alpha/Amount to set the fade to.
	* @param Color - Color of the fade.
	* @param bFadeAudio - True to fade audio volume along with the alpha of the solid color.
	*/
	UFUNCTION(BlueprintCallable, Category = "Camera Fades", meta = (WorldContext = "ContextObject", Keywords = "Fade Camera"))
		static void SetCameraFade(const UObject* ContextObject, const float Alpha, const FLinearColor Color, const bool bFadeAudio);

	/* Does a camera fade to/from a solid color. Animates automatically.
	* @param FromAlpha - Alpha at which to begin the fade. Range [0..1], where 0 is fully transparent and 1 is fully opaque solid color.
	* @param ToAlpha - Alpha at which to finish the fade.
	* @param Duration - How long the fade should take, in seconds.
	* @param Color - Color to fade to/from.
	* @param bShouldFadeAudio - True to fade audio volume along with the alpha of the solid color.
	* @param bHoldWhenFinished - True for fade to hold at the ToAlpha until explicitly stopped (e.g. with StopCameraFade).
	*/
	UFUNCTION(BlueprintCallable, Category = "Camera Fades", meta = (WorldContext = "ContextObject", Keywords = "Fade Camera", AdvancedDisplay = "Color, bHoldWhenFinished"))
		static void StartCameraFade(UObject* ContextObject, const float FromAlpha, const float ToAlpha = 1.0f,
			const float Duration = 1.0f, const FLinearColor Color = FLinearColor::Black,
			const bool bShouldFadeAudio = true, const bool bHoldWhenFinished = true);

	/* Set the view target blending with variable control.
	* @param NewTarget - New actor to set as view target.
	* @param BlendTime - Time taken to blend.
	* @param BlendFunc - Cubic, Linear etc functions for blending.
	* @param BlendExp -  Exponent, used by certain blend functions to control the shape of the curve.
	* @param bLockOutgoing - If true, lock outgoing view target to last frame's camera position for the remainder of the blend.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game|Player", meta = (WorldContext = "ContextObject", DefaultToSelf = "NewTarget", Keywords = "Camera", AdvancedDisplay = "BlendFunc, BlendExp, bLockOutgoing"))
		static void SetViewTarget(UObject* ContextObject, AActor* NewTarget,
			const float BlendTime = 1.0f, const EViewTargetBlendFunction BlendFunc = VTBlend_Linear,
			const float BlendExp = 0.0f, const bool bLockOutgoing = false);

	/* Adds an actor tag. */
	UFUNCTION(BlueprintCallable, Category = "Actor", DisplayName = "Add Tag", meta = (DefaultToSelf = "Target"))
		static void AddActorTag(AActor* Target, const FName InTag);

	/* Removes an actor tag. */
	UFUNCTION(BlueprintCallable, Category = "Actor", DisplayName = "Remove Tag", meta = (DefaultToSelf = "Target"))
		static void RemoveActorTag(AActor* Target, const FName InTag);
	
	/* Plays a Level Sequence Actor. */
	UFUNCTION(BlueprintCallable, Category = "LevelSequence", DisplayName = "Play", meta = (DefaultToSelf = "Target"))
		static void PlayLevelSequence(const ALevelSequenceActor* Target);

	/* Stops a Level Sequence Actor. */
	UFUNCTION(BlueprintCallable, Category = "LevelSequence", DisplayName = "Stop", meta = (DefaultToSelf = "Target"))
		static void StopLevelSequence(const ALevelSequenceActor* Target);

	/* Skips a Level Sequence Actor. */
	UFUNCTION(BlueprintCallable, Category = "LevelSequence", DisplayName = "Skip", meta = (DefaultToSelf = "Target"))
		static void SkipLevelSequence(const ALevelSequenceActor* Target);
	
	/* Binds to a delegate that gets fired once the Level Sequence finishes playing. */
	UFUNCTION(BlueprintCallable, Category = "LevelSequence", meta = (DefaultToSelf = "Target"))
		static void BindOnSequenceFinished(const ALevelSequenceActor* Target, const FRCBasicDelegate& Callback);

	/* Unbinds from a delegate that gets fired once the Level Sequence finishes playing. */
	UFUNCTION(BlueprintCallable, Category = "LevelSequence", meta = (DefaultToSelf = "Target"))
		static void UnbindOnSequenceFinished(const ALevelSequenceActor* Target, const FRCBasicDelegate& Callback);

	/* Plays an Ambient Sound actor with optional Start Time. */
	UFUNCTION(BlueprintCallable, Category = "AmbientSound", DisplayName = "Play", meta = (DefaultToSelf = "Target"))
		static void PlayAmbientSound(const AAmbientSound* Target, const float StartTime = 0.0f);

	/* Stops an Ambient Sound actor with optional Delay Time. */
	UFUNCTION(BlueprintCallable, Category = "AmbientSound", DisplayName = "Stop", meta = (DefaultToSelf = "Target"))
		static void StopAmbientSound(const AAmbientSound* Target, const float DelayTime = 0.0f);
	
	/* Fades In an Ambient Sound actor with optional Start Time. */
	UFUNCTION(BlueprintCallable, Category = "AmbientSound", DisplayName = "Fade In", meta = (DefaultToSelf = "Target"))
		static void FadeInAmbientSound(const AAmbientSound* Target, const float FadeTime = 1.0f, const float FadeVolume = 1.0f, const float StartTime = 0.0f);

	/* Fades Out Ambient Sound actor. */
	UFUNCTION(BlueprintCallable, Category = "AmbientSound", DisplayName = "Fade Out", meta = (DefaultToSelf = "Target"))
		static void FadeOutAmbientSound(const AAmbientSound* Target, const float FadeTime = 1.0f, const float FadeVolume = 0.0f);

	/* Adjust an Ambient Sound actor's volume level smoothly. */
	UFUNCTION(BlueprintCallable, Category = "AmbientSound", DisplayName = "Adjust Volume", meta = (DefaultToSelf = "Target"))
		static void AdjustAmbientSoundVolume(const AAmbientSound* Target, const float FadeTime = 1.0f, const float FadeVolume = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Actor", meta = (DeterminesOutputType = "InClass", DynamicOutputParam = "ReturnValue"))
		static UActorComponent* AddActorInstanceComponent(AActor* Target, const TSubclassOf<UActorComponent> InClass);
};
