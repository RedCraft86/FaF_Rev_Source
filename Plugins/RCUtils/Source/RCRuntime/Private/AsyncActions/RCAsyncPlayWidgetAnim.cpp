// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "AsyncActions/RCAsyncPlayWidgetAnim.h"
#include "Animation/UMGSequencePlayer.h"

URCAsyncPlayWidgetAnim* URCAsyncPlayWidgetAnim::PlayWidgetAnimation(UUserWidget* Target, UWidgetAnimation* InAnimation, const float PlaybackSpeed, const EUMGSequencePlayMode::Type PlayDirection)
{
	URCAsyncPlayWidgetAnim* Task = NewObject<URCAsyncPlayWidgetAnim>();
	Task->_Target = Target;
	Task->_Animation = InAnimation;
	Task->_PlaybackSpeed = FMath::Max(PlaybackSpeed, 0.0f);
	Task->_PlayDirection = PlayDirection;
	return Task;
}

void URCAsyncPlayWidgetAnim::Activate()
{
	if (!_Target)
	{
		FFrame::KismetExecutionMessage(TEXT("Target widget is null!"), ELogVerbosity::Warning);
		return;
	}

	if (!_Animation)
	{
		FFrame::KismetExecutionMessage(TEXT("Animation is null!"), ELogVerbosity::Warning);
		return;
	}

	SequencePlayer = _Target->PlayAnimation(_Animation, 0.0f, 1, _PlayDirection, _PlaybackSpeed);
	if (SequencePlayer)
	{
		SequencePlayer->OnSequenceFinishedPlaying().AddUObject(this, &URCAsyncPlayWidgetAnim::OnFinishedPlaying);
	}
	else
	{
		OnCompleted.Broadcast();
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void URCAsyncPlayWidgetAnim::OnFinishedPlaying(UUMGSequencePlayer& InPlayer)
{
	if (SequencePlayer)
	{
		SequencePlayer->OnSequenceFinishedPlaying().RemoveAll(this);
	}

	SequencePlayer = nullptr;
	OnCompleted.Broadcast();
}
