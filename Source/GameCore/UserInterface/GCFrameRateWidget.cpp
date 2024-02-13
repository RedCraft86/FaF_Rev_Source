// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCFrameRateWidget.h"
#include "Components/TextBlock.h"
#include "UserSettings/GCUserSettings.h"

UGCFrameRateWidget::UGCFrameRateWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	FrameRateText = nullptr;
	DeltaTimeText = nullptr;
	ProgressedTime = 1.0f;
}

FLinearColor UGCFrameRateWidget::GetFrameRateColor(const float InFrameRate) const
{
	static const UGCUserSettings* Settings;
	if (!Settings) Settings = UGCUserSettings::Get();

	float Limit = Settings->GetFrameRateLimit();
	if (FMath::IsNearlyZero(Limit)) Limit = 144.0f;

	return FMath::Lerp(FLinearColor::Red, FLinearColor::Green,
		FMath::GetMappedRangeValueClamped(FVector2D{Limit * 0.25f, Limit},
			FVector2D{0.0f, 1.0f}, InFrameRate));
}

void UGCFrameRateWidget::NativeTick(const FGeometry& InGeometry, float DeltaTime)
{
	Super::NativeTick(InGeometry, DeltaTime);
	ProgressedTime += DeltaTime;
	if (ProgressedTime >= 0.25f)
	{
		const int FrameRate = FMath::RoundToInt(1.0f / DeltaTime);
		FrameRateText->SetText(FText::FromString(FString::Printf(TEXT("%d FPS"), FrameRate)));
		DeltaTimeText->SetText(FText::FromString(FString::Printf(TEXT("%.2f ms"), DeltaTime * 1000.0f)));

		const FSlateColor Color = GetFrameRateColor(FrameRate);
		FrameRateText->SetColorAndOpacity(Color);
		DeltaTimeText->SetColorAndOpacity(Color);
		
		ProgressedTime = 0.0f;
	}
}
