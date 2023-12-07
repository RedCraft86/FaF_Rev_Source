// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "GCLoadingWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/RichTextBlock.h"
#include "Player/GCPlayerController.h"
#include "Messaging/GCMessageWidget.h"
#include "Gameplay/GCGameplayWidget.h"
#include "Gameplay/GCNarrativeWidget.h"
#include "GameSequence/GCSequenceManager.h"
#include "Player/GCPlayerCharacter.h"

UGCLoadingWidget::UGCLoadingWidget(const FObjectInitializer& ObjectInitializer)
	: UGCUserWidget(ObjectInitializer)
{
	WidgetZOrder = 15;
	bAutoAddToViewport = false;

	BackgroundImage = nullptr;
	SequenceLabel = nullptr;
	TextLabel = nullptr;
	TextDesc = nullptr;
	LoadingBar = nullptr;
	DefaultBackgroundImage = nullptr;
	LoadingTexts = {};

	SequenceManager = nullptr;
	RandomMax = 0.0f;
	TargetPercent = 0.0f;
	bLoadInProgress = false;
}

UTexture2D* UGCLoadingWidget::GetBackgroundImage_Implementation(FGCSequenceData& Data)
{
	BackgroundImg = Data.GetRandomBackground();
	return BackgroundImg.IsNull() ? DefaultBackgroundImage : BackgroundImg.LoadSynchronous();
}

void UGCLoadingWidget::UpdateProgress()
{
	if (bLoadInProgress && TargetPercent < RandomMax)
	{
		TargetPercent = FMath::Clamp(FMath::FRandRange(TargetPercent,
			TargetPercent + FMath::FRandRange(0.025f, 0.2f)), 0.0f, RandomMax);
		
		GetWorld()->GetTimerManager().SetTimer(ProgressTimer, this, &UGCLoadingWidget::UpdateProgress,
			FMath::FRandRange(0.05f, 0.2f), false);
	}
}

void UGCLoadingWidget::OnSequenceChangeStart(const FName& ID)
{
	if (ID.IsNone())
	{
		return;
	}
	
	bLoadInProgress = true;
	TargetPercent = 0.0f;
	RandomMax = FMath::FRandRange(0.9f, 0.98f);
	LoadingBar->SetPercent(0.0f);
	
	FGCSequenceData Data = UGCSequenceManager::GetGameSequenceData(ID);
	BackgroundImage->SetBrushFromTexture(GetBackgroundImage(Data), false);
	SequenceLabel->SetText(FText::FromString(TEXT("Now Loading: " + (Data.Label.IsEmpty() ? TEXT("None") : Data.Label))));
	
	if (!LoadingTexts.IsEmpty())
	{
		TArray<FString> Keys; LoadingTexts.GenerateKeyArray(Keys);
		const int32 Idx = FMath::RandRange(0, Keys.Num() - 1);
		TextLabel->SetText(FText::FromString(Keys[Idx]));
		if (TextDesc)
		{
			TextDesc->SetText(LoadingTexts.FindRef(Keys[Idx]));
		}
		else
		{
			SetTextDesc(LoadingTexts.FindRef(Keys[Idx]));
		}
	}

	LoadingBar->SetPercent(0.0f);
	AddWidget([this](){
		UpdateProgress();
	}, true);

	PlayerController->GetUserWidget<UGCNarrativeWidget>()->SetWidgetHidden(true);
	PlayerController->GetUserWidget<UGCGameplayWidget>()->SetWidgetHidden(true);
	PlayerController->GetUserWidget<UGCMessageWidget>()->SetWidgetHidden(true);
	PlayerController->SetShowMouseCursor(true);
}

void UGCLoadingWidget::OnSequenceChangeFinish(const FName& ID)
{
	if (ID.IsNone())
	{
		return;
	}
	
	GetWorld()->GetTimerManager().ClearTimer(ProgressTimer);
	
	bLoadInProgress = false;
	TargetPercent = 1.0f;

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, [this](){
		PlayerController->GetUserWidget<UGCNarrativeWidget>()->SetWidgetHidden(false);
		PlayerController->GetUserWidget<UGCGameplayWidget>()->SetWidgetHidden(false);
		PlayerController->GetUserWidget<UGCMessageWidget>()->SetWidgetHidden(false);
		RemoveWidget([this](){
			BackgroundImg = nullptr;
		}, true);
	}, 1.0f, false);
	
	PlayerController->SetShowMouseCursor(false);
}

void UGCLoadingWidget::InitData(AGCPlayerController* Controller)
{
	Super::InitData(Controller);
	Controller->GetPlayer()->LoadingWidget = this;
	SequenceManager = GetWorld()->GetSubsystem<UGCSequenceManager>();
	if (SequenceManager)
	{
		SequenceManager->OnSequenceChangeStart.AddUObject(this, &UGCLoadingWidget::OnSequenceChangeStart);
		SequenceManager->OnSequenceChangeFinish.AddUObject(this, &UGCLoadingWidget::OnSequenceChangeFinish);
	}
}

void UGCLoadingWidget::NativeTick(const FGeometry& Geometry, float DeltaTime)
{
	Super::NativeTick(Geometry, DeltaTime);
	if (LoadingBar->GetPercent() != TargetPercent)
	{
		LoadingBar->SetPercent(FMath::FInterpConstantTo(LoadingBar->GetPercent(), TargetPercent, DeltaTime, 2.0f));
	}
}
