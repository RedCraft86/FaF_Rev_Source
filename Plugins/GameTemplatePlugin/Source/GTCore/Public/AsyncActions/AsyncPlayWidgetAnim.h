// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "ExtendedAsyncAction.h"
#include "Blueprint/UserWidget.h"
#include "AsyncPlayWidgetAnim.generated.h"

UCLASS()
class GTCORE_API UAsyncPlayWidgetAnim final : public UExtendedAsyncAction
{
	GENERATED_BODY()

public:

	/* Plays a widget animation.
	* @param Target - User Widget to play the animation on.
	* @param InAnimation - Animation to play.
	* @param PlaybackSpeed - The speed multiplier to play the animation with. Minimum: 0.05f
	* @param PlayDirection - Direction to play the animation to.
	*/
	UFUNCTION(BlueprintCallable, Category = "UserInterface|Animation", meta = (BlueprintInternalUseOnly = "true", DefaultToSelf = "Target", DisplayName = "Play Widget Animation"))
		static UAsyncPlayWidgetAnim* PlayWidgetAnimation(UUserWidget* Target, UWidgetAnimation* InAnimation,
			const float PlaybackSpeed = 1.0f, const EUMGSequencePlayMode::Type PlayDirection = EUMGSequencePlayMode::Forward);

	UPROPERTY(BlueprintAssignable)
		FAsyncActionDelegate OnFinished;
	
private:

	UPROPERTY(Transient) TObjectPtr<UUserWidget> _Target = nullptr;
	UPROPERTY(Transient) TObjectPtr<UWidgetAnimation> _Animation = nullptr;
	UPROPERTY(Transient) TObjectPtr<UUMGSequencePlayer> SequencePlayer = nullptr;
	EUMGSequencePlayMode::Type _PlayDirection = EUMGSequencePlayMode::Forward;
	float _PlaybackSpeed = 1.0f;
	
	virtual void Activate() override;
	void OnFinishPlaying(UUMGSequencePlayer& InPlayer);
};
