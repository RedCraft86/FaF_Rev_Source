// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "PhaseMapWidget.generated.h"

UCLASS(Abstract)
class FAF_REV_API UPhaseMapWidgetBase final : public UGTUserWidget
{
	GENERATED_BODY()

public:

	UPhaseMapWidgetBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, BlueprintReadOnly, Category = "PhaseMapWidget")
		TObjectPtr<UUserWidget> ParentWidget;
};
