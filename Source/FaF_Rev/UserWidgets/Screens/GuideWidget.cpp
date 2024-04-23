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

#define GUIDE_TABLE FRSettings->GuideTable.LoadSynchronous()

FGuideBookPageData FindPageData(const FName& Row)
{
	FGuideBookPageData PageData;
	const UDataTable* Table = GUIDE_TABLE;
	if (!Table) return FGuideBookPageData();
	if (const FGuideBookPageData* Ptr = Table->FindRow<FGuideBookPageData>(Row, Table->GetName()))
	{
		PageData = *Ptr;
	}
	return PageData;
}

UGuideWidgetBase::UGuideWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), ImageHeightRange({35.0f}), NextButton(nullptr)
	, GuideSwitcher(nullptr), CustomPageContainer(nullptr), LocalPageTitle(nullptr)
	, LocalPageText(nullptr), LocalPageImage(nullptr), LocalImageContainer(nullptr)
	, GuideFadeAnim(nullptr), NextButtonAnim(nullptr), bActive(false), bPaused(false), SkipTimer(0)
{
	DefaultImage = LoadObject<UTexture2D>( nullptr, TEXT("/Engine/EngineResources/DefaultTexture.DefaultTexture")); 
}

void UGuideWidgetBase::QueueGuide(const FGuideBookPageID PageID)
{
	if (!PageID.IsValid()) return;
	if (!PageQueue.IsEmpty() && *PageQueue.Peek() == *PageID) return;
	
	PageQueue.Enqueue(*PageID);
	if (!bActive)
	{
		bActive = true;
		ProceedNextGuide();
		
		bPaused = UGameplayStatics::IsGamePaused(this);
		UGameplayStatics::SetGamePaused(this, true);
	}
}

void UGuideWidgetBase::QueueGuides(const TArray<FGuideBookPageID>& PageIDs)
{
	for (const FGuideBookPageID& ID : PageIDs)
	{
		QueueGuide(ID);
	}
}

void UGuideWidgetBase::ProceedNextGuide()
{
	Reset();
	FName PageID;
	if (PageQueue.Dequeue(PageID))
	{
		const FGuideBookPageData PageData = FindPageData(PageID);
		if (PageData.IsValidData())
		{
			if (PageData.CustomWidget.IsNull())
			{
				GuideSwitcher->SetActiveWidgetIndex(0);
				LocalPageTitle->SetText(PageData.Label);
				LocalPageText->SetText(PageData.Description, true);
				if (!PageData.Image.IsNull())
				{
					if (ImageHeightRange.X > 0)
					{
						LocalImageContainer->SetMinDesiredHeight(ImageHeightRange.X);
					}
					
					if (ImageHeightRange.Y > ImageHeightRange.X)
					{
						LocalImageContainer->SetMaxDesiredHeight(ImageHeightRange.Y);
					}
					
					LocalPageImage->SetBrushFromTexture(PageData.Image.LoadSynchronous());
				}
			}
			else
			{
				GuideSwitcher->SetActiveWidgetIndex(1);
				CustomPageContainer->AddChild(CreateWidget(this,
					PageData.CustomWidget.LoadSynchronous()));
			}
			
			PlayAnimation(GuideFadeAnim);
			SkipTimer = PageData.WaitTime;
			return;
		}

		ProceedNextGuide();
	}

	bActive = false;
	UGameplayStatics::SetGamePaused(this, bPaused);
	PlayAnimationReverse(GuideFadeAnim);
}

void UGuideWidgetBase::Reset()
{
	NextButton->SetIsEnabled(true);
	PlayAnimation(NextButtonAnim, 0, 1, EUMGSequencePlayMode::Reverse);
	LocalPageText->SetText(FText::GetEmpty(), true);
	LocalPageTitle->SetText(FText::GetEmpty());
	LocalPageImage->SetBrushFromTexture(DefaultImage);
	LocalImageContainer->ClearMinDesiredHeight();
	LocalImageContainer->ClearMaxDesiredHeight();
	CustomPageContainer->ClearChildren();
}

void UGuideWidgetBase::OnNextClicked()
{
	NextButton->SetIsEnabled(false);
	if (UUMGSequencePlayer* P = PlayAnimation(GuideFadeAnim, 0, 1, EUMGSequencePlayMode::Reverse))
	{
		P->OnSequenceFinishedPlaying().AddLambda([this](UUMGSequencePlayer&)
		{
			ProceedNextGuide();
		});
	}
}

void UGuideWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	NextButton->SetIsEnabled(false);
	NextButton->OnClicked.AddDynamic(this, &UGuideWidgetBase::OnNextClicked);
}

void UGuideWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (SkipTimer > 0.0f)
	{
		SkipTimer -= InDeltaTime;
	}
	else if (bActive != NextButton->GetIsEnabled())
	{
		NextButton->SetIsEnabled(bActive);
	}
}
