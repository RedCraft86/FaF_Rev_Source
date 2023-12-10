// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "GCLightAnimator.h"

UGCLightAnim_Intensity::UGCLightAnim_Intensity()
{
	AnimationCurve.GetRichCurve()->AddKey(0.0f, 1.0f);
	AnimationCurve.GetRichCurve()->AddKey(1.0f, 0.5f);
	OutputRange = {0.0f, 1.0f};
	PlayRate = 1.0f;
	bLooping = false;
	
	TimeRange = FVector2D::ZeroVector;
	ValueRange = FVector2D::ZeroVector;
	PlaybackTime = 0.0f;
	CurrentValue = 1.0f;
	ScaledValue = 1.0f;
}

#if WITH_EDITOR
void UGCLightAnim_Intensity::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	Init();
#if WITH_EDITORONLY_DATA
	PreviewTime = FMath::Clamp(PreviewTime, TimeRange.X, TimeRange.Y);
	PreviewValue = FMath::GetMappedRangeValueClamped(ValueRange, OutputRange,
		AnimationCurve.GetRichCurveConst()->Eval(PreviewTime));
#endif
}
#endif

void UGCLightAnim_Intensity::Init()
{
	float X, Y = 0.0f;
	AnimationCurve.GetRichCurveConst()->GetTimeRange(X, Y);
	TimeRange = {X, Y};

	AnimationCurve.GetRichCurveConst()->GetValueRange(X, Y);
	ValueRange = {X, Y};

	PlaybackTime = TimeRange.X;
	CurrentValue = 1.0f;
	ScaledValue = 1.0f;
}

void UGCLightAnim_Intensity::Tick(const float DeltaTime)
{
	if (bPlaying)
	{
		CurrentValue = AnimationCurve.GetRichCurveConst()->Eval(PlaybackTime);
		ScaledValue = FMath::GetMappedRangeValueClamped(ValueRange, OutputRange, CurrentValue);

		PlaybackTime += DeltaTime * PlayRate;
		if (PlaybackTime > TimeRange.Y)
		{
			PlaybackTime = TimeRange.X;
			if (!bLooping)
			{
				StopAnimation();
			}
		}
	}
}

void UGCLightAnim_Intensity::StartAnimation(const bool bReset)
{
	if (!bPlaying || bReset)
	{
		if (bReset) { Init(); }
		Super::StartAnimation(bReset);
	}
}

void UGCLightAnim_Intensity::StopAnimation()
{
	if (bPlaying)
	{
		ScaledValue = 1.0f;
		Super::StopAnimation();
	}
}
