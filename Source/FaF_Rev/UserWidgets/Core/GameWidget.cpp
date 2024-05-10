// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameWidget.h"

UGameWidgetBase::UGameWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), PauseFadeAnim(nullptr), WorldSettings(nullptr)
{
	ZOrder = 95;
	bAutoAdd = true;
}

void UGameWidgetBase::PauseCheck()
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

void UGameWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	WorldSettings = GetWorld()->GetWorldSettings();
	GetWorld()->GetTimerManager().SetTimer(PauseCheckTimer, this, &UGameWidgetBase::PauseCheck, 0.25f, true);
}
