// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameWidget.h"

UGameWidgetBase::UGameWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer)
{
	ZOrder = 95;
	bAutoAdd = true;
}
