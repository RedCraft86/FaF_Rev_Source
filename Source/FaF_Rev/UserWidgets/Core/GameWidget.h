// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "GameWidget.generated.h"

UCLASS(Abstract)
class FAF_REV_API UGameWidgetBase final : public UGTUserWidget
{
	GENERATED_BODY()

public:

	UGameWidgetBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> HideFadeAnim;

protected:

	float HideCheckTime;
	UPROPERTY(Transient) TObjectPtr<const AWorldSettings> WorldSettings;
	UPROPERTY(Transient) TObjectPtr<class AFRPlayerBase> PlayerChar;

	void HideCheck();
	virtual void InitWidget() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
