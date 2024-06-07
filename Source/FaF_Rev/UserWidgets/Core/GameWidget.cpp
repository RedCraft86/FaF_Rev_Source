﻿// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "FRPlayer.h"
#include "FRSettings.h"

UGameWidgetBase::UGameWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), InteractIcon(nullptr), InteractLabel(nullptr), StaminaBar(nullptr)
	, HideFadeAnim(nullptr), DefaultInteractIcon(nullptr), DefaultInteractSize(35.0f), StaminaBarSpeed(0.2f, 5.0f)
	, bInitFade(false), bAutoHidden(false), HideCheckTime(0.0f), WorldSettings(nullptr), PlayerChar(nullptr)
{
	ZOrder = 92;
	bAutoAdd = true;
}

void UGameWidgetBase::HideCheck()
{
	HideCheckTime = 0.0f;
	if (!PlayerChar || !WorldSettings || IsAnimationPlaying(HideFadeAnim)) return;
	const bool bNewAutoHidden = PlayerChar->GetActiveCutscene() || WorldSettings->GetPauserPlayerState();
	if (bAutoHidden != bNewAutoHidden)
	{
		bAutoHidden = bNewAutoHidden;
		bAutoHidden ? PlayAnimationForward(HideFadeAnim) : PlayAnimationReverse(HideFadeAnim);
	}
}

void UGameWidgetBase::InteractCheck() const
{
	FHitResult HitResult;
	FPlayerInteraction InteractionData;
	if (PlayerChar && PlayerChar->TraceInteraction(HitResult, InteractionData))
	{
		FSlateBrush Brush = InteractIcon->GetBrush();
		const bool bHasIcon = IsValid(InteractionData.InteractInfo.Icon);
		Brush.SetImageSize(bHasIcon ? InteractionData.InteractInfo.IconSize : DefaultInteractSize);
		Brush.SetResourceObject(bHasIcon ? InteractionData.InteractInfo.Icon : DefaultInteractIcon);
		InteractIcon->SetBrush(Brush);
		
		InteractLabel->SetText(InteractionData.InteractInfo.Label);
		OffsetInteractLabel(InteractLabel, Brush.GetImageSize().Size() / DefaultInteractSize.Size());
		
		Crosshair->SetVisibility(bHasIcon ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
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
	if (!FRSettings->IsGameplayMap(this))
	{
		SetVisibility(ESlateVisibility::Collapsed);
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
			FVector2D(1.0f, 0.25f), StaminaBar->GetPercent()));

		StaminaBar->SetFillColorAndOpacity(FMath::CInterpTo(StaminaBar->GetFillColorAndOpacity(),
			PlayerChar->IsStaminaPunished() ? FLinearColor::Red : FLinearColor::White,
			InDeltaTime, StaminaBarSpeed.Y));
	}
}
