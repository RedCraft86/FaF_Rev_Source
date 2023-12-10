// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "GCFrameRateWidget.h"
#include "Components/TextBlock.h"

UGCFrameRateWidget::UGCFrameRateWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	FrameRateText = nullptr;
	DeltaTimeText = nullptr;
	ProgressedTime = 1.0f;
}

void UGCFrameRateWidget::NativeTick(const FGeometry& InGeometry, float DeltaTime)
{
	Super::NativeTick(InGeometry, DeltaTime);
	ProgressedTime += DeltaTime;
	if (ProgressedTime >= 0.25f)
	{
		FrameRateText->SetText(FText::FromString(FString::Printf(TEXT("%d FPS"), FMath::RoundToInt(1.0f / DeltaTime))));
		DeltaTimeText->SetText(FText::FromString(FString::Printf(TEXT("%.2f ms"), DeltaTime * 1000.0f)));
		ProgressedTime = 0.0f;
	}
}
