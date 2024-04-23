// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "NarrativeWidget.generated.h"

UCLASS(Abstract)
class FAF_REV_API UNarrativeWidget final : public UGTUserWidget
{
	GENERATED_BODY()
	
public:

	UNarrativeWidget(const FObjectInitializer& ObjectInitializer);
};
