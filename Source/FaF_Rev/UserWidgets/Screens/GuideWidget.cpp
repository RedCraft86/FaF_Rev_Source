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
#include "FRGameMode.h"

UGuideWidgetBase::UGuideWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), NextButton(nullptr), TypeSwitch(nullptr)
	, CustomPageContainer(nullptr), LocalPageTitle(nullptr), LocalPageText(nullptr)
	, LocalPageImage(nullptr), LocalImageContainer(nullptr), GuideFadeAnim(nullptr), NextButtonAnim(nullptr)
	, ImageHeight(0.0f), DefaultImage(nullptr), bActive(false), bPrePauseState(false)
{
	ZOrder = 97;
	bAutoAdd = false;
}

void UGuideWidgetBase::QueuePage(const FGuideBookPageID& PageID)
{
	if (!PageID.IsValid()) return;
	PageQueue.Enqueue(PageID);
	if (!bActive)
	{
		AddWidget(nullptr);
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
					LocalImageContainer->SetVisibility(ESlateVisibility::HitTestInvisible);
					LocalImageContainer->SetHeightOverride(ImageHeight);
					FSlateBrush Brush = LocalPageImage->GetBrush();
					Brush.SetResourceObject(Data.Image.LoadSynchronous());
					LocalPageImage->SetBrush(Brush);
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
		GetGameMode<AFRGameModeBase>()->SetInputModeData(PreInputMode);
		UGameplayStatics::SetGamePaused(this, bPrePauseState);
		bActive = false;
	}
}

void UGuideWidgetBase::ResetBook() const
{
	NextButton->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	NextButton->SetRenderOpacity(0.0f);
	CustomPageContainer->ClearChildren();
	LocalPageTitle->SetText(FText::GetEmpty());
	LocalPageText->SetText(FText::GetEmpty(), true);
	LocalImageContainer->ClearHeightOverride();
	LocalImageContainer->SetVisibility(ESlateVisibility::Collapsed);

	FSlateBrush Brush = LocalPageImage->GetBrush();
	Brush.SetResourceObject(DefaultImage);
	LocalPageImage->SetBrush(Brush);
}

void UGuideWidgetBase::OnNextClicked()
{
	NextButton->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
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

	if (!bActive)
	{
		ResetBook();
		PreInputMode = GetGameMode<AFRGameModeBase>()->GetInputModeData();
		GetGameMode<AFRGameModeBase>()->SetGameInputMode(EGameInputMode::GameAndUI, true,
			EMouseLockMode::LockAlways, false, this);
	}

	bActive = true;
	if (!PageQueue.IsEmpty())
	{
		bPrePauseState = UGameplayStatics::IsGamePaused(this);
		UGameplayStatics::SetGamePaused(this, true);
		ProceedNextGuide();
	}
	else
	{
		GetGameMode<AFRGameModeBase>()->SetInputModeData(PreInputMode);
		UGameplayStatics::SetGamePaused(this, bPrePauseState);
		bActive = false;
	}
}
