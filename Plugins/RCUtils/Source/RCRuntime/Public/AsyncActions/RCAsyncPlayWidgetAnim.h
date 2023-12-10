// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "RCAsyncActionBase.h"
#include "Animation/UMGSequencePlayer.h"
#include "Blueprint/UserWidget.h"
#include "RCAsyncPlayWidgetAnim.generated.h"

UCLASS()
class RCRUNTIME_API URCAsyncPlayWidgetAnim final : public URCAsyncActionBase
{
	GENERATED_BODY()

public:

	/* Plays a widget animation.
	* @param Target - User Widget to play the animation on.
	* @param InAnimation - Animation to play.
	* @param PlaybackSpeed - The speed multiplier to play the animation with.
	* @param PlayDirection - Direction to play the animation to.
	*/
	UFUNCTION(BlueprintCallable, Category = "UserInterface|Animation", meta = (BlueprintInternalUseOnly = "true", DefaultToSelf = "Target", DisplayName = "Play Widget Animation"))
		static URCAsyncPlayWidgetAnim* PlayWidgetAnimation(UUserWidget* Target, UWidgetAnimation* InAnimation, const float PlaybackSpeed = 1.0f, const EUMGSequencePlayMode::Type PlayDirection = EUMGSequencePlayMode::Forward);

	UPROPERTY(BlueprintAssignable)
		FRCAsyncActionDelegate OnCompleted;
	
	virtual void Activate() override;
	
private:

	UPROPERTY(Transient)
		UUserWidget* _Target = nullptr;

	UPROPERTY(Transient)
		UWidgetAnimation* _Animation = nullptr;

	UPROPERTY(Transient)
		UUMGSequencePlayer* SequencePlayer = nullptr;

	float _PlaybackSpeed = 1.0f;
	EUMGSequencePlayMode::Type _PlayDirection = EUMGSequencePlayMode::Forward;

	void OnFinishedPlaying(UUMGSequencePlayer& InPlayer);
};
