// Copyright (C) RedCraft86. All Rights Reserved.

#include "GuideWidget.h"

#include "ExprTextBlock.h"
#include "Animation/UMGSequencePlayer.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"


// #define GUIDE_TABLE FRSettings->GuideTable.LoadSynchronous()

// FGuideBookPageData FindPageData(const FName& Row)
// {
// 	FGuideBookPageData PageData;
// 	const UDataTable* Table = GUIDE_TABLE;
// 	if (!Table) return FGuideBookPageData();
// 	if (const FGuideBookPageData* Ptr = Table->FindRow<FGuideBookPageData>(Row, Table->GetName()))
// 	{
// 		PageData = *Ptr;
// 	}
// 	return PageData;
// }
//
// UGuideWidgetBase::UGuideWidgetBase(const FObjectInitializer& ObjectInitializer)
// 	: UGTUserWidget(ObjectInitializer), ImageHeightRange({35.0f}), NextButton(nullptr)
// 	, GuideSwitcher(nullptr), CustomPageContainer(nullptr), LocalPageTitle(nullptr)
// 	, LocalPageText(nullptr), LocalPageImage(nullptr), LocalImageContainer(nullptr)
// 	, GuideFadeAnim(nullptr), NextButtonAnim(nullptr), bActive(false), bPaused(false), SkipTimer(0)
// {
// 	ZOrder = 100;
// 	DefaultImage = LoadObject<UTexture2D>( nullptr, TEXT("/Engine/EngineResources/DefaultTexture.DefaultTexture")); 
// }
//
// void UGuideWidgetBase::QueueGuide(const FGuideBookPageID PageID)
// {
// 	if (!PageID.IsValid()) return;
// 	if (!PageQueue.IsEmpty() && *PageQueue.Peek() == *PageID) return;
// 	
// 	PageQueue.Enqueue(*PageID);
// 	if (!bActive)
// 	{
// 		bActive = true;
// 		ProceedNextGuide();
// 		
// 		bPaused = UGameplayStatics::IsGamePaused(this);
// 		UGameplayStatics::SetGamePaused(this, true);
// 	}
// }
//
// void UGuideWidgetBase::QueueGuides(const TArray<FGuideBookPageID>& PageIDs)
// {
// 	for (const FGuideBookPageID& ID : PageIDs)
// 	{
// 		QueueGuide(ID);
// 	}
// }
//
// void UGuideWidgetBase::ProceedNextGuide()
// {
// 	Reset();
// 	FName PageID;
// 	if (PageQueue.Dequeue(PageID))
// 	{
// 		const FGuideBookPageData PageData = FindPageData(PageID);
// 		if (PageData.IsValidData())
// 		{
// 			if (PageData.CustomWidget.IsNull())
// 			{
// 				GuideSwitcher->SetActiveWidgetIndex(0);
				// LocalPageTitle->SetText(PageData.Label);
				// LocalPageText->SetText(PageData.Description, true);
				// if (!PageData.Image.IsNull())
				// {
				// 	if (ImageHeightRange.X > 0)
				// 	{
				// 		LocalImageContainer->SetMinDesiredHeight(ImageHeightRange.X);
				// 	}
				// 	
				// 	if (ImageHeightRange.Y > ImageHeightRange.X)
				// 	{
				// 		LocalImageContainer->SetMaxDesiredHeight(ImageHeightRange.Y);
				// 	}
				// 	
				// 	LocalPageImage->SetBrushFromTexture(PageData.Image.LoadSynchronous());
				// }
// 			}
// 			else
// 			{
// 				GuideSwitcher->SetActiveWidgetIndex(1);
// 				CustomPageContainer->AddChild(CreateWidget(this,
// 					PageData.CustomWidget.LoadSynchronous()));
// 			}
// 			
// 			PlayAnimation(GuideFadeAnim);
// 			SkipTimer = PageData.WaitTime;
// 			NextButton->SetIsEnabled(false);
// 			NextButton->SetVisibility(ESlateVisibility::Visible);
// 			return;
// 		}
//
// 		ProceedNextGuide();
// 		return;
// 	}
//
// 	bActive = false;
// 	UGameplayStatics::SetGamePaused(this, bPaused);
// 	PlayAnimationReverse(GuideFadeAnim);
// }
//
// void UGuideWidgetBase::Reset()
// {
// 	NextButton->SetVisibility(ESlateVisibility::Collapsed);
// 	PlayAnimation(NextButtonAnim, 0, 1, EUMGSequencePlayMode::Reverse);
// 	LocalPageText->SetText(FText::GetEmpty(), true);
// 	LocalPageTitle->SetText(FText::GetEmpty());
// 	LocalPageImage->SetBrushFromTexture(DefaultImage);
// 	LocalImageContainer->ClearMinDesiredHeight();
// 	LocalImageContainer->ClearMaxDesiredHeight();
// 	CustomPageContainer->ClearChildren();
// }

// void UGuideWidgetBase::NativeConstruct()
// {
// 	Super::NativeConstruct();
// 	NextButton->SetVisibility(ESlateVisibility::Collapsed);
// 	NextButton->OnClicked.AddDynamic(this, &UGuideWidgetBase::OnNextClicked);
// }
//

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
	TypeSwitch->SetActiveWidgetIndex(0);
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
