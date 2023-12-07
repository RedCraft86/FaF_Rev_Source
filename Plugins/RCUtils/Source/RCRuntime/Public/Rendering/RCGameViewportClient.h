// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "Engine/GameViewportClient.h"
#include "RCGameViewportClient.generated.h"

struct FRCViewportFader
{
private:
	
	float Current;
	float Target;
	float Duration;
	bool bFadeAudio;
	bool bIsDirtyAudio;

public:

	FRCViewportFader() : Current(0.0f), Target(0.0f), Duration(0.0f), bFadeAudio(false), bIsDirtyAudio(false) {}

	void Update(const float DeltaTime)
	{
		if (Duration > 0.0f && !FMath::IsNearlyZero(Duration))
		{
			Current = FMath::FInterpConstantTo(Current, Target, DeltaTime, 1.0f / Duration);
		}
		else
		{
			Current = Target;
		}
	}

	FORCEINLINE void UnmarkAudioDirty() { bIsDirtyAudio = false; }
	FORCEINLINE void StartFade(const float InTarget, const float InDuration, const bool bInFadeAudio)
	{
		bIsDirtyAudio = bFadeAudio ? !bInFadeAudio : false;
		
		Target = FMath::Clamp(InTarget, 0.0f, 1.0f);
		Duration = FMath::Max(InDuration, 0.0f);
		bFadeAudio = bInFadeAudio;
	}

	FORCEINLINE float GetAlpha() const { return FMath::Clamp(Current, 0.0f, 1.0f); }
	FORCEINLINE bool ShouldFadeAudio() const { return bFadeAudio; }
	FORCEINLINE bool IsAudioDirty() const { return bIsDirtyAudio; }
};

UCLASS()
class RCRUNTIME_API URCGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

public:

	static URCGameViewportClient* Get(const UObject* WorldContext);
	
	virtual void PostRender(UCanvas* Canvas) override;

	const FRCViewportFader& GetFader() const { return Fader; }
	void StartViewportFade(const float Target, const float Duration, const bool bFadeAudio);
	void SetManualViewportFade(const float Target, const bool bFadeAudio);
	void StopViewportFade();

protected:

	FRCViewportFader Fader;
};
