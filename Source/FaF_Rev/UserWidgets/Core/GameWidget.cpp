// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameWidget.h"
#include "FRPlayer.h"

UGameWidgetBase::UGameWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), HideFadeAnim(nullptr), WorldSettings(nullptr), PlayerChar(nullptr)
{
	ZOrder = 95;
	bAutoAdd = true;
}

void UGameWidgetBase::HideCheck()
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

void UGameWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	WorldSettings = GetWorld()->GetWorldSettings();
	GetWorld()->GetTimerManager().SetTimer(HideCheckTimer, this, &UGameWidgetBase::HideCheck, 0.25f, true);
}
