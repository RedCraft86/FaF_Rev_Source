// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "GCGameplayWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Player/GCPlayerCharacter.h"
#include "Player/GCPlayerController.h"

UGCGameplayWidget::UGCGameplayWidget(const FObjectInitializer& ObjectInitializer)
	: UGCUserWidget(ObjectInitializer)
{
	WidgetZOrder = 10;
	bAutoAddToViewport = true;
	
	StaminaBar = nullptr;
	CrosshairText = nullptr;
	StaminaAnim = nullptr;
	CrosshairAnim = nullptr;

	Player = nullptr;
	bCachedStaminaState = false;
	bCachedInteractState = false;
}

void UGCGameplayWidget::UpdateInteraction()
{
	if (Player)
	{
		FGCInteractionData Data;
		if (!Player->AreEyesClosed())
		{
			FHitResult HitResult;
			Player->TraceInteraction(HitResult, Data);
		}
		
		if (!Data.Actor || Data.DisplayName.IsEmptyOrWhitespace())
		{
			CrosshairText->SetVisibility(ESlateVisibility::Collapsed);
			CrosshairText->SetText(FText::GetEmpty());
		}
		else
		{
			CrosshairText->SetVisibility(ESlateVisibility::HitTestInvisible);
			CrosshairText->SetText(Data.DisplayName);
		}

		if (bCachedInteractState != IsValid(Data.Actor))
		{
			bCachedInteractState = IsValid(Data.Actor);
			if (bCachedInteractState)
			{
				PlayAnimation(CrosshairAnim);
			}
			else
			{
				PlayAnimationReverse(CrosshairAnim);
			}
		}
	}
}

void UGCGameplayWidget::InitData(AGCPlayerController* Controller)
{
	Super::InitData(Controller);
	Player = Controller->GetPlayer();
}

void UGCGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StaminaBar->SetPercent(1.0f);
	CrosshairText->SetText(FText::GetEmpty());

	PLAY_FIRST_FRAME(StaminaAnim)
	PLAY_FIRST_FRAME(CrosshairAnim)

	if (!InteractTimer.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(InteractTimer, this, &UGCGameplayWidget::UpdateInteraction,
			0.1f, true, 0.0f);
	}
}

void UGCGameplayWidget::NativeDestruct()
{
	Super::NativeDestruct();
	GetWorld()->GetTimerManager().ClearTimer(InteractTimer);
}

void UGCGameplayWidget::NativeTick(const FGeometry& Geometry, float DeltaTime)
{
	Super::NativeTick(Geometry, DeltaTime);
	if (Player)
	{
		StaminaBar->SetPercent(FMath::FInterpConstantTo(StaminaBar->GetPercent(),
			Player->GetStaminaPercent(), DeltaTime, 0.2f));
		
		StaminaBar->SetFillColorAndOpacity(FMath::CInterpTo(StaminaBar->GetFillColorAndOpacity(),
			Player->IsStaminaPunished() ? FLinearColor::Red : FLinearColor::White, DeltaTime, 7.5f));

		if (bCachedStaminaState != StaminaBar->GetPercent() <= 0.95f)
		{
			bCachedStaminaState = StaminaBar->GetPercent() <= 0.95f;
			if (bCachedStaminaState)
			{
				PlayAnimation(StaminaAnim);
			}
			else
			{
				PlayAnimationReverse(StaminaAnim);
			}
		}
	}
}
