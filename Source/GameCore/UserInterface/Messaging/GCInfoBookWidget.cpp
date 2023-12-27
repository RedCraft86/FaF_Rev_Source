// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCInfoBookWidget.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UserInterface/Gameplay/GCGameplayWidget.h"
#include "UserInterface/Gameplay/GCNarrativeWidget.h"
#include "Player/GCPlayerController.h"
#include "Core/GCSettings.h"
#include "GCMessageWidget.h"

UGCInfoBookWidget::UGCInfoBookWidget(const FObjectInitializer& ObjectInitializer)
	: UGCUserWidget(ObjectInitializer)
{
	WidgetZOrder = 14;
	bAutoAddToViewport = false;
	
	ContinueButton = nullptr;
	LabelText = nullptr;
	MessageBox = nullptr;
	ContentImage = nullptr;
	ContentBox = nullptr;
	ContentFadeAnim = nullptr;
	ImageMsgHeight = 200.0f;

	PageTable = nullptr;
	ActiveWidget = nullptr;
	CurrentPage = NAME_None;
	ContinueWaitTime = 0.0f;
}

void UGCInfoBookWidget::QueuePage(const FGCInfoPageID PageID)
{
	if (PageID.IsValid())
	{
		AddWidget(nullptr, true);
		
		Pages.Enqueue(*PageID);
		if (CurrentPage.IsNone())
		{
			OnContinueClicked();
		}
	}
}

void UGCInfoBookWidget::QueuePages(const TArray<FGCInfoPageID>& PageIDs)
{
	for (const FGCInfoPageID& ID : PageIDs)
	{
		QueuePage(ID);
	}
}

void UGCInfoBookWidget::NativeConstruct()
{
	Super::NativeConstruct();
	PlayerController->GetUserWidget<UGCMessageWidget>()->SetWidgetHidden(true);
	PlayerController->GetUserWidget<UGCGameplayWidget>()->SetWidgetHidden(true);
	PlayerController->GetUserWidget<UGCNarrativeWidget>()->SetWidgetHidden(true);
			
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, this,
		EMouseLockMode::LockAlways, false);
			
	PlayerController->SetShowMouseCursor(true);
	PlayerController->SetPause(true);
}

void UGCInfoBookWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	PageTable = UGCSettings::Get()->InfoPageTable.LoadSynchronous();
	ContinueButton->OnClicked.AddDynamic(this, &UGCInfoBookWidget::OnContinueClicked);
	ContinueButton->SetIsEnabled(false);
}

void UGCInfoBookWidget::NativeTick(const FGeometry& InGeometry, float InDeltaTime)
{
	Super::NativeTick(InGeometry, InDeltaTime);
	if (ContinueWaitTime > 0.0f)
	{
		ContinueWaitTime -= InDeltaTime;
		if (ContinueWaitTime <= 0.0f)
		{
			ContinueButton->SetIsEnabled(true);
			ContinueText->SetVisibility(ESlateVisibility::HitTestInvisible);
			PlayAnimation(ContinueFadeAnim);
		}
	}
}

void UGCInfoBookWidget::RemoveWidget(const TFunction<void()>& OnFinished, const bool bFade)
{
	Super::RemoveWidget(OnFinished, bFade);

	if (bIsRemoving)
	{
		PlayerController->GetUserWidget<UGCMessageWidget>()->SetWidgetHidden(false);
		PlayerController->GetUserWidget<UGCGameplayWidget>()->SetWidgetHidden(false);
		PlayerController->GetUserWidget<UGCNarrativeWidget>()->SetWidgetHidden(false);
		
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
		PlayerController->SetShowMouseCursor(false);
		PlayerController->SetPause(false);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGCInfoBookWidget::OnContinueClicked()
{
	ContinueButton->SetIsEnabled(false);
	ContinueText->SetVisibility(ESlateVisibility::Collapsed);
	if (Pages.Dequeue(CurrentPage) && PageTable)
	{
		if (const FGCInfoPageData* Data = PageTable->FindRow<FGCInfoPageData>(CurrentPage, GetName()))
		{
			LabelText->SetText(Data->Label);
			BackgroundContent->SetVisibility(Data->bShowBackground ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

			WidgetBox->ClearChildren();
			if (ActiveWidget)
			{
				ActiveWidget->MarkAsGarbage();
				ActiveWidget = nullptr;
			}
			
			if (!Data->Widget)
			{
				if (ActiveWidget)
				{
					ActiveWidget->MarkAsGarbage();
					ActiveWidget = nullptr;
				}
				
				ActiveWidget = CreateWidget<UUserWidget>(this, Data->Widget);
				WidgetBox->AddChild(ActiveWidget);
			}
			else
			{
				ContentImage->SetBrushFromTexture(Data->Image.LoadSynchronous());
				ContentBox->SetVisibility(IsValid(ContentImage->GetBrush().GetResourceObject()) ?
					ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);

				const bool bEmptyText = Data->HasEmptyMessage();
				SetMessageText(Data->GetMessage(), bEmptyText ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
			
				if (Data->Image && !bEmptyText)
				{
					MessageBox->SetHeightOverride(ImageMsgHeight);
				}
				else
				{
					MessageBox->ClearHeightOverride();
				}
			}

			PlayAnimation(ContentFadeAnim);
			ContinueWaitTime = FMath::Max(Data->MinWaitTime, 0.1f);
		}
		else
		{
			OnContinueClicked();
		}
	}
	else
	{
		Pages.Empty();
		RemoveWidget(nullptr, true);

		OnInfoBookExited.Broadcast();
		OnInfoBookExitedBP.Broadcast();
	}
}
