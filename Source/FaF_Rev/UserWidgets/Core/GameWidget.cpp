// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameWidget.h"
#include "FRPlayer.h"

UGameWidgetBase::UGameWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), HideFadeAnim(nullptr), HideCheckTime(0.0f)
	, WorldSettings(nullptr), PlayerChar(nullptr)
{
	ZOrder = 95;
	bAutoAdd = true;
}

void UGameWidgetBase::HideCheck()
{
	HideCheckTime = 0.0f;
	if (!PlayerChar || !WorldSettings) return;
	if (PlayerChar->GetActiveCutscene() || WorldSettings->GetPauserPlayerState())
	{
		PlayAnimationForward(HideFadeAnim);
	}
	else if (!IsAnimationPlaying(HideFadeAnim) && GetAnimationCurrentTime(HideFadeAnim) > 0.1f)
	{
		PlayAnimationReverse(HideFadeAnim);
	}
}

void UGameWidgetBase::InitWidget()
{
	WorldSettings = GetWorld()->GetWorldSettings();
	PlayerChar = FRPlayer(this);
}

void UGameWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (HideCheckTime >= 0.25f) { HideCheck(); }
	else { HideCheckTime += InDeltaTime; }
}
