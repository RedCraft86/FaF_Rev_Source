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
		UTextBlock* FrameRateText;

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* DeltaTimeText;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animations", meta = (BindWidgetAnim))
		UWidgetAnimation* SaveAnim;

private:

	FTimerHandle FrameRateTimer;

	void UpdateInfo();
	void OnSettingsUpdate();
	void FrameRateTick() const;
	virtual void InitWidget() override;
};
