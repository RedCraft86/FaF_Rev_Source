﻿// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "PhotoModeWidget.generated.h"

UCLASS(Abstract)
class FAF_REV_API UPhotoModeWidgetBase final : public UGTUserWidget
{
	GENERATED_BODY()

	friend class AFRGameModeBase;

public:

	UPhotoModeWidgetBase(const FObjectInitializer& ObjectInitializer);
	
	void BeginCapture();
	
protected:

	UPROPERTY(Transient) UUserWidget* ParentWidget;
	UPROPERTY(Transient) class APhotoModeActor* PhotoModeActor;
};
