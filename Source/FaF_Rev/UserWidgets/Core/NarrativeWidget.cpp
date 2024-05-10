// Copyright (C) RedCraft86. All Rights Reserved.

#include "NarrativeWidget.h"
#include "FRPlayer.h"

UNarrativeWidget::UNarrativeWidget(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), HideFadeAnim(nullptr), HideCheckTime(0.0f)
	, WorldSettings(nullptr), PlayerChar(nullptr)
{
	ZOrder = 97;
	bAutoAdd = true;
}

void UNarrativeWidget::HideCheck()
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

void UNarrativeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	WorldSettings = GetWorld()->GetWorldSettings();
	PlayerChar = FRPlayer(this);
}

void UNarrativeWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (HideCheckTime >= 0.25f) { HideCheck(); }
	else { HideCheckTime += InDeltaTime; }
}
