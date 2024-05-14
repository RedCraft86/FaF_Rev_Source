// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "InventoryWidget.generated.h"

UCLASS()
class FAF_REV_API UInventoryWidgetBase final : public UGTUserWidget
{
	GENERATED_BODY()

public:

	UInventoryWidgetBase(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
