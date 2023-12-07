// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "GCUserWidget.h"
#include "Player/GCPlayerController.h"
#include "Animation/UMGSequencePlayer.h"

UGCUserWidget::UGCUserWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	FadeAnim = nullptr;
	
	WidgetZOrder = 0;
	bAutoAddToViewport = true;

	PlayerController = nullptr;
	bShouldBeHidden = false;
	bIsRemoving = false;
}

UGCUserWidget* UGCUserWidget::Create(AGCPlayerController* Controller, const TSubclassOf<UGCUserWidget> Class)
{
	if (Controller && Class)
	{
		if (UGCUserWidget* Widget = CreateWidget<UGCUserWidget>(Controller, Class))
		{
			Widget->InitData(Controller);
			if (Widget->bAutoAddToViewport)
			{
				Widget->AddWidget(nullptr, true);
			}
			return Widget;
		}
	}

	return nullptr;
}

void UGCUserWidget::SetWidgetHidden(const bool bInHidden)
{
	if (!IsInViewport())
	{
		bShouldBeHidden = false;
		return;
	}
	
	if (bShouldBeHidden != bInHidden)
	{
		bShouldBeHidden = bInHidden;
		if (bShouldBeHidden)
		{
			PlayAnimation(FadeAnim, 0.0f, 1, EUMGSequencePlayMode::Reverse);
		}
		else
		{
			PlayAnimation(FadeAnim);
		}
	}
}

void UGCUserWidget::AddUserWidget(const bool bFade)
{
	AddWidget(nullptr, bFade);
}

void UGCUserWidget::RemoveUserWidget(const bool bFade)
{
	RemoveWidget(nullptr, bFade);
}

void UGCUserWidget::AddWidget(const TFunction<void()>& OnFinished, const bool bFade)
{
	if (!bIsRemoving && !IsInViewport())
	{
		AddToViewport(WidgetZOrder);
		if (bFade)
		{
			if (UUMGSequencePlayer* Player = PlayAnimation(FadeAnim))
			{
				FDelegateHandle Handle;
				Handle = Player->OnSequenceFinishedPlaying().AddLambda([this, Player, &Handle, OnFinished](UUMGSequencePlayer&){
					bShouldBeHidden = false;
					if (OnFinished)
						OnFinished();

					Player->OnSequenceFinishedPlaying().Remove(Handle);
				});
			}
		}
		else if (OnFinished)
		{
			OnFinished();
		}
	}
}

void UGCUserWidget::RemoveWidget(const TFunction<void()>& OnFinished, const bool bFade)
{
	if (!bIsRemoving && IsInViewport())
	{
		bIsRemoving = true;
		bShouldBeHidden = false;
		if (bFade)
		{
			if (UUMGSequencePlayer* Player = PlayAnimationReverse(FadeAnim))
			{
				FDelegateHandle Handle;
				Handle = Player->OnSequenceFinishedPlaying().AddLambda([this, Player, &Handle, OnFinished](UUMGSequencePlayer&){
					RemoveFromParent();
					bIsRemoving = false;
					if (OnFinished)
						OnFinished();

					Player->OnSequenceFinishedPlaying().Remove(Handle);
				});
			}
		}
		else
		{
			RemoveFromParent();
			bIsRemoving = true;
			if (OnFinished)
				OnFinished();
		}
	}
}

void UGCUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	PLAY_FIRST_FRAME(FadeAnim);
	bShouldBeHidden = true;
}

void UGCUserWidget::InitData(AGCPlayerController* Controller)
{
	PlayerController = Controller;
}
