// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "FRPlayer.h"
#include "FRSettings.h"

UGameWidgetBase::UGameWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), InteractIcon(nullptr), InteractLabel(nullptr), StaminaBar(nullptr)
	, HideFadeAnim(nullptr), StaminaBarSpeed(0.25f, 5.0f), bInitFade(false), HideCheckTime(0.0f)
	, WorldSettings(nullptr), PlayerChar(nullptr)
{
	ZOrder = 92;
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

void UGameWidgetBase::InteractCheck() const
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
	if (FRSettings->IsGameplayMap(this))
	{
		SetWidgetHidden(true);
	}
	
	GetWorld()->GetTimerManager().SetTimer(InteractCheckHandle, this,
		&UGameWidgetBase::InteractCheck, 0.05f, true);
}

void UGameWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (HideCheckTime >= 0.25f) { HideCheck(); }
	else { HideCheckTime += InDeltaTime; }

	if (PlayerChar && PlayerChar->GetStaminaPercent() != StaminaBar->GetPercent())
	{
		StaminaBar->SetPercent(FMath::FInterpConstantTo(StaminaBar->GetPercent(),
			PlayerChar->GetStaminaPercent(), InDeltaTime, StaminaBarSpeed.X));

		StaminaBar->SetRenderOpacity(FMath::GetMappedRangeValueClamped(FVector2D(0.85f, 1.0f),
			FVector2D(1.0f, 0.05f), StaminaBar->GetPercent()));

		StaminaBar->SetFillColorAndOpacity(FMath::CInterpTo(StaminaBar->GetFillColorAndOpacity(),
			PlayerChar->IsStaminaPunished() ? FLinearColor::Red : FLinearColor::White,
			InDeltaTime, StaminaBarSpeed.Y));
	}
}
