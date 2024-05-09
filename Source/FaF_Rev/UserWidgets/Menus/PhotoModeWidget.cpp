// Copyright (C) RedCraft86. All Rights Reserved.

#include "PhotoModeWidget.h"

UPhotoModeWidgetBase::UPhotoModeWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), ParentWidget(nullptr)
{
	ZOrder = 98;
	bAutoAdd = false;
}

void UPhotoModeWidgetBase::BeginCapture()
{
}
