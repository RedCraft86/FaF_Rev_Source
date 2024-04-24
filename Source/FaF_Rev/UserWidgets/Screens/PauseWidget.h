// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "PauseWidget.generated.h"

UCLASS(Abstract)
class FAF_REV_API UPauseWidgetBase final : public UGTUserWidget
{
	GENERATED_BODY()
	
public:

	UPauseWidgetBase(const FObjectInitializer& ObjectInitializer);
};
