// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "InfoWidget.generated.h"

class UTextBlock;

UCLASS(Abstract)
class FAF_REV_API UInfoWidgetBase final : public UGTUserWidget
{
	GENERATED_BODY()
	
public:

	UInfoWidgetBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> FrameRateText;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> DeltaTimeText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> SaveAnim;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FLinearColor GoodFrameRateColor;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FLinearColor BadFrameRateColor;

private:

	float TargetFPS;
	float FPSTickTime;
	bool bWantsFPS;

	void OnSaveStarted();
	void OnSettingsUpdate();
	void FrameRateTick() const;
	virtual void InitWidget() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
