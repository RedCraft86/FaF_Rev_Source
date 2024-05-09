// Copyright (C) RedCraft86. All Rights Reserved.

#include "PhotoModeWidget.h"

UPhotoModeWidgetBase::UPhotoModeWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer)
{
	ZOrder = 98;
	bAutoAdd = false;
}