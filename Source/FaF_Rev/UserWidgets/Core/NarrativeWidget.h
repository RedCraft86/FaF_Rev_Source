﻿// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "NarrativeWidget.generated.h"

UCLASS(Abstract)
class FAF_REV_API UNarrativeWidget final : public UGTUserWidget
{
	GENERATED_BODY()
	
public:

	UNarrativeWidget(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animations", meta = (BindWidgetAnim))
	UWidgetAnimation* PauseFadeAnim;

protected:

	FTimerHandle PauseCheckTimer;
	UPROPERTY(Transient) AWorldSettings* WorldSettings;

	void PauseCheck();
	virtual void NativeConstruct() override;
};
