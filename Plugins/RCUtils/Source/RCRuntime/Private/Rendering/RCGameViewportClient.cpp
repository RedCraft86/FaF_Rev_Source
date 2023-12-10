// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "Rendering/RCGameViewportClient.h"
#include "Engine/Canvas.h"
#include "AudioDevice.h"

URCGameViewportClient* URCGameViewportClient::Get(const UObject* WorldContext)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	return World ? Cast<URCGameViewportClient>(World->GetGameViewport()) : nullptr;
}

void URCGameViewportClient::PostRender(UCanvas* Canvas)
{
	Super::PostRender(Canvas);

	Fader.Update(GetWorld()->GetDeltaSeconds());
	if (Fader.GetAlpha() > 0.0f || Fader.IsAudioDirty())
	{
		Canvas->SetDrawColor(FLinearColor(0.0f, 0.0f, 0.0f, Fader.GetAlpha()).ToFColorSRGB());
		Canvas->DrawTile(Canvas->DefaultTexture, 0.0f, 0.0f, Canvas->ClipX, Canvas->ClipY, 0.0f, 0.0f,
			(float)Canvas->DefaultTexture->GetSizeX(), (float)Canvas->DefaultTexture->GetSizeY());
	
		if (AudioDevice.IsValid())
		{
			if (Fader.ShouldFadeAudio())
			{
				AudioDevice->SetTransientPrimaryVolume(FMath::Lerp(1.0f, 0.0f, Fader.GetAlpha()));
			}
			else if (Fader.IsAudioDirty())
			{
				AudioDevice->SetTransientPrimaryVolume(1.0f);
				Fader.UnmarkAudioDirty();
			}
		}
	}
}

void URCGameViewportClient::StartViewportFade(const float Target, const float Duration, const bool bFadeAudio)
{
	Fader.StartFade(Target, Duration, bFadeAudio);
}

void URCGameViewportClient::SetManualViewportFade(const float Target, const bool bFadeAudio)
{
	Fader.StartFade(Target, 0.0f, bFadeAudio);
}

void URCGameViewportClient::StopViewportFade()
{
	Fader.StartFade(0.0f, 0.0f, false);
}
