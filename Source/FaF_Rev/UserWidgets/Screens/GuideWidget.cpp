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

UGuideWidgetBase::UGuideWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), NextButton(nullptr), TypeSwitch(nullptr)
	, CustomPageContainer(nullptr), LocalPageTitle(nullptr), LocalPageText(nullptr)
	, LocalPageImage(nullptr), LocalImageContainer(nullptr), GuideFadeAnim(nullptr), NextButtonAnim(nullptr)
	, ImageHeight(0), DefaultImage(nullptr), bActive(false), bPrePauseState(false)
{
	ZOrder = 99;
	bAutoAdd = false;
}

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
		RemoveWidget(nullptr);
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

void UGuideWidgetBase::InitWidget()
{
	Super::InitWidget();
	NextButton->OnClicked.AddDynamic(this, &UGuideWidgetBase::OnNextClicked);
}

void UGuideWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	ResetBook();
}
