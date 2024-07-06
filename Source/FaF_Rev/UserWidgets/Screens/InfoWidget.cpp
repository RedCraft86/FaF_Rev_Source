// Copyright (C) RedCraft86. All Rights Reserved.

#include "InfoWidget.h"
#include "Components/TextBlock.h"
#include "GameSettings/GameSettings.h"
#include "SaveSystem/SaveSubsystem.h"

FLinearColor CalcFrameRateColor(const FLinearColor& Good, const FLinearColor& Bad, const float Target, const float Current)
{
	return FMath::Lerp(Bad, Good, FMath::GetMappedRangeValueClamped(
			FVector2D(0.0f, (FMath::IsNearlyZero(Target) ? 60.0f : Target) - 10.0f),
			FVector2D(0.0f, 1.0f), Current));
}

UInfoWidgetBase::UInfoWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), FrameRateText(nullptr), DeltaTimeText(nullptr), SaveAnim(nullptr)
	, GoodFrameRateColor(FLinearColor::Green), BadFrameRateColor(FLinearColor::Red), TargetFPS(60.0f)
	, FPSTickTime(0.0f), bWantsFPS(false)
{
	ZOrder = 99;
	bAutoAdd = true;
}

void UInfoWidgetBase::OnSaveStarted()
{
	PlayAnimation(SaveAnim);
	OnSettingsUpdate();
}

void UInfoWidgetBase::OnSettingsUpdate()
{
	const UGameSettings* Settings = UGameSettings::Get();
	TargetFPS = Settings->GetFrameRateLimit();
	if (TargetFPS > 150.0f) TargetFPS = 60.0f;
	bWantsFPS = Settings->GetShowFPS();
	FrameRateTick();

	FrameRateText->SetVisibility(bWantsFPS ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	DeltaTimeText->SetVisibility(bWantsFPS ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}

void UInfoWidgetBase::FrameRateTick() const
{
	const int32 FPS = FMath::RoundToInt32(1.0 / FApp::GetDeltaTime());
	const FLinearColor Color = CalcFrameRateColor(GoodFrameRateColor, BadFrameRateColor, TargetFPS, FPS);
	
	FrameRateText->SetText(FText::FromString(FString::Printf(TEXT("%d FPS"), FPS)));
	FrameRateText->SetColorAndOpacity(Color);

	DeltaTimeText->SetText(FText::FromString(FString::Printf(TEXT("%.2f ms"), FApp::GetDeltaTime() * 1000.0f)));
	DeltaTimeText->SetColorAndOpacity(Color);
}

void UInfoWidgetBase::InitWidget()
{
	OnSettingsUpdate();

	USaveSubsystem::Get(this)->OnSaveStarted.AddUObject(this, &UInfoWidgetBase::OnSaveStarted);

	UGameSettings* Settings = UGameSettings::Get();
	Settings->OnManualApply.AddUObject(this, &UInfoWidgetBase::OnSaveStarted);
	Settings->OnDynamicApply.AddUObject(this, &UInfoWidgetBase::OnSettingsUpdate);
}

void UInfoWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!bWantsFPS) return;
	if (FPSTickTime >= 0.1f)
	{
		FPSTickTime = 0.0f;
		FrameRateTick();
	}
	else { FPSTickTime += InDeltaTime; }
}
