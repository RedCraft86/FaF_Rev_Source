// Copyright (C) RedCraft86. All Rights Reserved.

#include "GuideWidget.h"
#include "ExprTextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/UMGSequencePlayer.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UGuideWidgetBase::QueuePage(const FGuideBookPageID& PageID)
{
	if (!PageID.IsValid()) return;
	const bool bWasEmpty = PageQueue.IsEmpty();
	PageQueue.Enqueue(PageID);
	if (bWasEmpty)
	{
		bActive = true;
		bPrePauseState = UGameplayStatics::IsGamePaused(this);
		ProceedNextGuide();
	}
}

void UGuideWidgetBase::QueuePages(const TArray<FGuideBookPageID>& PageIDs)
{
	for (const FGuideBookPageID& ID : PageIDs)
	{
		QueuePage(ID);
	}
}

void UGuideWidgetBase::ProceedNextGuide()
{
	ResetBook();
	FGuideBookPageID PageID;
	if (PageQueue.Dequeue(PageID))
	{
		if (const FGuideBookPageData Data = PageID.GetPageData(); Data.IsValidData())
		{
			if (Data.CustomWidget.IsNull())
			{
				TypeSwitch->SetActiveWidgetIndex(0);
				LocalPageTitle->SetText(Data.Label);
				LocalPageText->SetText(Data.Description, true);
				if (!Data.Image.IsNull())
				{
					LocalImageContainer->SetHeightOverride(ImageHeight);
					LocalPageImage->SetBrushFromTexture(Data.Image.LoadSynchronous());
				}
			}
			else
			{
				TypeSwitch->SetActiveWidgetIndex(1);
				CustomPageContainer->AddChild(CreateWidget(this,
					Data.CustomWidget.LoadSynchronous()));
			}
			
			PlayAnimation(GuideFadeAnim);
			PlayAnimation(NextButtonAnim);
		}
		else
		{
			ProceedNextGuide();
		}
	}
	else
	{
		PlayAnimationReverse(GuideFadeAnim, 2.0f);
		UGameplayStatics::SetGamePaused(this, bPrePauseState);
		bActive = false;
	}
}

void UGuideWidgetBase::ResetBook() const
{
	NextButton->SetIsEnabled(false);
	NextButton->SetRenderOpacity(0.0f);
	CustomPageContainer->ClearChildren();
	LocalPageTitle->SetText(FText::GetEmpty());
	LocalPageText->SetText(FText::GetEmpty(), true);
	LocalPageImage->SetBrushFromTexture(DefaultImage);
	LocalImageContainer->ClearHeightOverride();
}

void UGuideWidgetBase::OnNextClicked()
{
	NextButton->SetIsEnabled(false);
	PlayAnimationReverse(NextButtonAnim, 2.0f, true);
	if (UUMGSequencePlayer* Anim = PlayAnimationReverse(GuideFadeAnim))
	{
		Anim->OnSequenceFinishedPlaying().AddLambda([this](UUMGSequencePlayer&)
		{
			ProceedNextGuide();
		});
	}
}

void UGuideWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	NextButton->OnClicked.AddDynamic(this, &UGuideWidgetBase::OnNextClicked);
	ResetBook();
}

void UGuideWidgetBase::NativeDestruct()
{
	Super::NativeDestruct();
	if (NextButton) NextButton->OnClicked.RemoveDynamic(this, &UGuideWidgetBase::OnNextClicked);
}
