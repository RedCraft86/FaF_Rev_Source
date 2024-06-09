// Copyright (C) RedCraft86. All Rights Reserved.

#include "AsyncActions/AsyncPlayWidgetAnim.h"
#include "Animation/UMGSequencePlayer.h"
#include "Animation/WidgetAnimation.h"

UAsyncPlayWidgetAnim* UAsyncPlayWidgetAnim::PlayWidgetAnimation(UUserWidget* Target, UWidgetAnimation* InAnimation,
	const float PlaybackSpeed, const EUMGSequencePlayMode::Type PlayDirection)
{
	CREATE_TASK
	Task->_Target = Target;
	Task->_Animation = InAnimation;
	Task->_PlaybackSpeed = FMath::Max(PlaybackSpeed, 0.05f);
	Task->_PlayDirection = PlayDirection;
	return Task;
}

void UAsyncPlayWidgetAnim::Activate()
{
	CHECK_RUNNING
	
	if (!IsValid(_Target))
	{
		UE_KLOG_WARNING(2.0f, TEXT("Cannot run Async Action %s. Target User Widget is not provided."), *GetClass()->GetName())
		FINISH_RUNNING
		return;
	}

	if (!IsValid(_Animation))
	{
		UE_KLOG_WARNING(2.0f, TEXT("Cannot run Async Action %s. Animation is not provided."), *GetClass()->GetName())
		FINISH_RUNNING
		return;
	}

	SequencePlayer = _Target->PlayAnimation(_Animation, 0.0f, 1, _PlayDirection, _PlaybackSpeed);
	if (!IsValid(SequencePlayer))
	{
		UE_KLOG_WARNING(2.0f, TEXT("Cannot run Async Action %s. Sequence Player was null."), *GetClass()->GetName())
		FINISH_RUNNING
		return;
	}

	SequencePlayer->OnSequenceFinishedPlaying().AddUObject(this, &UAsyncPlayWidgetAnim::OnFinishPlaying);
}

void UAsyncPlayWidgetAnim::OnFinishPlaying(UUMGSequencePlayer& InPlayer)
{
	if (IsValid(SequencePlayer))
	{
		SequencePlayer->OnSequenceFinishedPlaying().RemoveAll(this);
	}

	SequencePlayer = nullptr;
	OnFinished.Broadcast();
	FINISH_RUNNING
}
