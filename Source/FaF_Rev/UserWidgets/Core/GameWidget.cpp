// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "FRPlayer.h"

UGameWidgetBase::UGameWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), InteractIcon(nullptr), InteractLabel(nullptr), StaminaBar(nullptr)
	, HideFadeAnim(nullptr), StaminaBarSpeed(2.0f), HideCheckTime(0.0f), WorldSettings(nullptr), PlayerChar(nullptr)
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

void UGameWidgetBase::InteractCheck()
{
	FHitResult HitResult;
	FPlayerInteraction InteractionData;
	if (PlayerChar && PlayerChar->TraceInteraction(HitResult, InteractionData))
	{
		InteractLabel->SetText(InteractionData.InteractInfo.Label);
		InteractIcon->SetBrushFromTexture(InteractionData.InteractInfo.Icon
			? InteractionData.InteractInfo.Icon : DefaultInteractIcon);

		Crosshair->SetVisibility(ESlateVisibility::Collapsed);
		InteractIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
		InteractLabel->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		Crosshair->SetVisibility(ESlateVisibility::HitTestInvisible);
		InteractIcon->SetVisibility(ESlateVisibility::Collapsed);
		InteractLabel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UGameWidgetBase::InitWidget()
{
	WorldSettings = GetWorld()->GetWorldSettings();
	PlayerChar = FRPlayer(this);
}

void UGameWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	GetWorld()->GetTimerManager().SetTimer(InteractCheckHandle, this,
		&UGameWidgetBase::InteractCheck, 0.05f, true);
}

void UGameWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (HideCheckTime >= 0.25f) { HideCheck(); }
	else { HideCheckTime += InDeltaTime; }

	if (PlayerChar && PlayerChar->IsStaminaChanging())
	{
		StaminaBar->SetPercent(FMath::FInterpConstantTo(StaminaBar->GetPercent(),
			PlayerChar->GetStaminaPercent(), InDeltaTime, StaminaBarSpeed));

		StaminaBar->SetRenderOpacity(FMath::GetMappedRangeValueClamped(FVector2D(0.85f, 1.0f),
			FVector2D(1.0f, 0.0f), StaminaBar->GetPercent()));
	}
}
