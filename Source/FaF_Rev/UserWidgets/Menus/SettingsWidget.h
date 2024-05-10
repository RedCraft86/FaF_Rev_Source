// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "PauseWidget.h"
#include "SettingsWidget.generated.h"

UCLASS(Abstract)
class FAF_REV_API USettingsWidgetBase final : public UGTUserWidget
{
	GENERATED_BODY()

public:

	USettingsWidgetBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SettingsWidget")
		UUserWidget* ParentWidget;
};
