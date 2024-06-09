// Copyright (C) RedCraft86. All Rights Reserved.

#include "GTUserWidget.h"
#include "GTPlayerController.h"
#include "Animation/UMGSequencePlayer.h"
#include "Animation/WidgetAnimation.h"

UGTUserWidget::UGTUserWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer), FadeAnim(nullptr), ZOrder(0), bAutoAdd(false)
	, WidgetTags({}), GameMode(nullptr), PlayerController(nullptr)
	, bRemoving(false), bWidgetHidden(false)
{
}

void UGTUserWidget::SetWidgetHidden(const bool bInHidden)
{
	if (!IsInViewport() || bRemoving) return;
	if (bWidgetHidden != bInHidden)
	{
		bWidgetHidden = bInHidden;
		if (IsValid(FadeAnim))
		{
			UUMGSequencePlayer* Player;
			if (bWidgetHidden)
			{
				Player = PlayAnimationReverse(FadeAnim);
			}
			else
			{
				Player = PlayAnimationForward(FadeAnim);
			}

			if (Player) Player->OnSequenceFinishedPlaying().Clear();
		}
		else
		{
			SetVisibility(bWidgetHidden ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
		}
	}
}

void UGTUserWidget::AddUserWidget(FGTWidgetCallbackSignature OnFinished)
{
	if (OnFinished.IsBound())
	{
		AddWidget([OnFinished]()
		{
			OnFinished.Execute();
		});
	}
	else AddWidget(nullptr);
}

void UGTUserWidget::RemoveUserWidget(FGTWidgetCallbackSignature OnFinished)
{
	if (OnFinished.IsBound())
	{
		RemoveWidget([OnFinished]()
		{
			OnFinished.Execute();
		});
	}
	else RemoveWidget(nullptr);
}

void UGTUserWidget::AddWidget(const TFunction<void()>& OnFinished)
{
	if (IsInViewport()) return;

	AddToViewport(ZOrder);
	if (FadeAnim)
	{
		if (UUMGSequencePlayer* Player = PlayAnimationForward(FadeAnim))
		{
			if (!OnFinished) return;
			Player->OnSequenceFinishedPlaying().Clear();
			Player->OnSequenceFinishedPlaying().AddLambda([OnFinished](UUMGSequencePlayer&)
			{
				if (OnFinished) OnFinished();
			});
		}
	}
	else if (OnFinished)
	{
		OnFinished();
	}
}

void UGTUserWidget::RemoveWidget(const TFunction<void()>& OnFinished)
{
	if (!IsInViewport() || bRemoving) return;

	bRemoving = true;
	bWidgetHidden = false;
	if (FadeAnim)
	{
		if (UUMGSequencePlayer* Player = PlayAnimationReverse(FadeAnim))
		{
			Player->OnSequenceFinishedPlaying().Clear();
			Player->OnSequenceFinishedPlaying().AddLambda([this, OnFinished](UUMGSequencePlayer&)
			{
				bRemoving = false;
				RemoveFromParent();
				if (OnFinished) OnFinished();
			});
		}
	}
	else
	{
		bRemoving = false;
		RemoveFromParent();
		if (OnFinished) OnFinished();
	}
}

/* Statics */

UGTUserWidget* UGTUserWidget::CreateSmartWidget(const UObject* ContextObject, APlayerController* Owner, const TSubclassOf<UGTUserWidget> Class, const FGameplayTagContainer Tags)
{
	APlayerController* Controller = Owner ? Owner : AGTPlayerController::Get(ContextObject);
	if (UGTUserWidget* Widget = CreateWidget<UGTUserWidget>(Controller, Class))
	{
		Widget->PlayerController = Controller;
		Widget->GameMode = Controller ? Controller->GetWorld()->GetAuthGameMode() : nullptr;
		Widget->WidgetTags.AppendTags(Tags);
		Widget->InitWidget();
		if (Widget->bAutoAdd)
			Widget->AddWidget(nullptr);

		return Widget;
	}

	return nullptr;
}
