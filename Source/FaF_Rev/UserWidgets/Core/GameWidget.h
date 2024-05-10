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

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animations", meta = (BindWidgetAnim))
		UWidgetAnimation* PauseFadeAnim;

protected:

	FTimerHandle PauseCheckTimer;
	UPROPERTY(Transient) AWorldSettings* WorldSettings;

	void PauseCheck();
	virtual void NativeConstruct() override;
};
