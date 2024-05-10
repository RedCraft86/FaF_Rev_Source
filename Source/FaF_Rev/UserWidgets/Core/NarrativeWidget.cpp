// Copyright (C) RedCraft86. All Rights Reserved.

#include "NarrativeWidget.h"
#include "FRPlayer.h"

UNarrativeWidget::UNarrativeWidget(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), HideFadeAnim(nullptr), WorldSettings(nullptr), PlayerChar(nullptr)
{
	ZOrder = 97;
	bAutoAdd = true;
}

void UNarrativeWidget::HideCheck()
{
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
	GetWorld()->GetTimerManager().SetTimer(HideCheckTimer, this, &UNarrativeWidget::HideCheck, 0.25f, true);
}
