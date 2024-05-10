// Copyright (C) RedCraft86. All Rights Reserved.

#include "NarrativeWidget.h"

UNarrativeWidget::UNarrativeWidget(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), PauseFadeAnim(nullptr), WorldSettings(nullptr)
{
	ZOrder = 97;
	bAutoAdd = true;
}

void UNarrativeWidget::PauseCheck()
{
	if (WorldSettings && WorldSettings->GetPauserPlayerState())
	{
		PlayAnimationForward(PauseFadeAnim);
	}
	else if (!IsAnimationPlaying(PauseFadeAnim) && GetAnimationCurrentTime(PauseFadeAnim) > 0.1f)
	{
		PlayAnimationReverse(PauseFadeAnim);
	}
}

void UNarrativeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	WorldSettings = GetWorld()->GetWorldSettings();
	GetWorld()->GetTimerManager().SetTimer(PauseCheckTimer, this, &UNarrativeWidget::PauseCheck, 0.25f, true);
}
