// Copyright (C) RedCraft86. All Rights Reserved.

#include "PhaseMapWidget.h"

UPhaseMapWidgetBase::UPhaseMapWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), ParentWidget(nullptr)
{
	ZOrder = 91;
	bAutoAdd = false;
}
