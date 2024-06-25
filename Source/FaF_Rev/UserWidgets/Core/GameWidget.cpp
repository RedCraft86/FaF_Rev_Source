// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "NarrativeComponent.h"
#include "ExprTextBlock.h"
#include "FRGameMode.h"
#include "FRSettings.h"
#include "FRPlayer.h"

UGameWidgetBase::UGameWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), InteractIcon(nullptr), InteractLabel(nullptr), StaminaBar(nullptr)
	, HideFadeAnim(nullptr), DefaultInteractIcon(nullptr), DefaultInteractSize(35.0f), StaminaBarSpeed(0.1f, 10.0f)
	, bInitFade(false), bAutoHidden(false), HideCheckTime(0.0f), WorldSettings(nullptr), PlayerChar(nullptr)
{
	ZOrder = 92;
	bAutoAdd = true;
}

void UGameWidgetBase::HideCheck()
{
	HideCheckTime = 0.0f;
	if (!PlayerChar || !WorldSettings || IsAnimationPlaying(HideFadeAnim)) return;
	const bool bNewAutoHidden = PlayerChar->GetActiveCutscene() || WorldSettings->GetPauserPlayerState()
		|| GetGameMode<AFRGameModeBase>()->Narrative->IsInDialogue();
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
		OffsetInteractLabel(InteractLabel, InteractionData.InteractInfo.LabelOffset,
			Brush.GetImageSize().Size() / DefaultInteractSize.Size());
		
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

	if (PlayerChar)
	{
		StaminaBar->SetPercent(FMath::FInterpConstantTo(StaminaBar->GetPercent(),
			PlayerChar->GetStaminaPercent(), InDeltaTime,
			FMath::Abs(PlayerChar->StaminaDelta) * StaminaBarSpeed.X));

		const bool bUseStamina = PlayerChar->HasControlFlag(PCF_CanRun) && PlayerChar->HasControlFlag(PCF_UseStamina);
		StaminaBar->SetRenderOpacity(FMath::GetMappedRangeValueClamped(FVector2D(0.85f, 1.0f),
			FVector2D(1.0f, bUseStamina ? 0.25f : 0.0f), StaminaBar->GetPercent()));

		StaminaBar->SetFillColorAndOpacity(FMath::CInterpTo(StaminaBar->GetFillColorAndOpacity(),
			PlayerChar->IsStaminaPunished() ? FLinearColor::Red : FLinearColor::White,
			InDeltaTime, StaminaBarSpeed.Y));
	}
}
