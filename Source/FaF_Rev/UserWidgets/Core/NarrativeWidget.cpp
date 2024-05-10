// Copyright (C) RedCraft86. All Rights Reserved.

#include "NarrativeWidget.h"
#include "FRGameMode.h"
#include "FRPlayer.h"

UNarrativeWidgetBase::UNarrativeWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), HideFadeAnim(nullptr), NarrativeComponent(nullptr)
	, HideCheckTime(0.0f), WorldSettings(nullptr), PlayerChar(nullptr)
{
	ZOrder = 97;
	bAutoAdd = true;
}

void UNarrativeWidgetBase::HideCheck()
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

void UNarrativeWidgetBase::InitWidget()
{
	NarrativeComponent = GetGameMode<AFRGameModeBase>()->Narrative;
	WorldSettings = GetWorld()->GetWorldSettings();
	PlayerChar = FRPlayer(this);
}

void UNarrativeWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (HideCheckTime >= 0.25f) { HideCheck(); }
	else { HideCheckTime += InDeltaTime; }
}
