﻿// Copyright (C) RedCraft86. All Rights Reserved.

#include "SettingsWidget.h"

USettingsWidgetBase::USettingsWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), ParentWidget(nullptr)
{
	ZOrder = 98;
	bAutoAdd = false;
}