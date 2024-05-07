// Copyright (C) RedCraft86. All Rights Reserved.

#include "InfoWidget.h"
#include "Components/TextBlock.h"
#include "SaveSystem/SaveSubsystem.h"

UInfoWidgetBase::UInfoWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), FrameRateText(nullptr), DeltaTimeText(nullptr), SaveAnim(nullptr)
{
	ZOrder = 100;
	bAutoAdd = true;
}

void UInfoWidgetBase::UpdateInfo()
{
	PlayAnimation(SaveAnim);

	const bool bWantsFPS = false; // getter
	FrameRateText->SetVisibility(bWantsFPS ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	DeltaTimeText->SetVisibility(bWantsFPS ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	
	bWantsFPS
		? GetWorld()->GetTimerManager().UnPauseTimer(FrameRateTimer)
		: GetWorld()->GetTimerManager().PauseTimer(FrameRateTimer);
}

void UInfoWidgetBase::FrameRateTick() const
{
	FrameRateText->SetText(FText::FromString(
		FString::Printf(TEXT("%d FPS"), FMath::RoundToInt32(1.0 / FApp::GetDeltaTime()))));
		
	DeltaTimeText->SetText(FText::FromString(
		FString::Printf(TEXT("%.2f ms"), FApp::GetDeltaTime() * 1000.0f)));
}

void UInfoWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	GetWorld()->GetTimerManager().SetTimer(FrameRateTimer,
		this, &UInfoWidgetBase::FrameRateTick, 0.25f, true);

	if (false) // getter
	{
		GetWorld()->GetTimerManager().PauseTimer(FrameRateTimer);
	}
}

void UInfoWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	USaveSubsystem::Get(this)->OnSaveStarted.AddUObject(this, &UInfoWidgetBase::UpdateInfo);
}
