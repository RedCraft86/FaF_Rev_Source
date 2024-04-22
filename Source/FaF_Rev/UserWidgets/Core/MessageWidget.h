// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "MessageWidget.generated.h"

class UExprTextBlock;

UCLASS(Abstract)
class FAF_REV_API UMessageWidgetBase final : public UGTUserWidget
{
	GENERATED_BODY()

private:
	
	UPROPERTY(Transient, meta = (BindWidget))
		UExprTextBlock* SmallNoticeLabel;

	UPROPERTY(Transient, meta = (BindWidget))
		UExprTextBlock* LargeNoticeLabel;

	UPROPERTY(Transient, meta = (BindWidget))
		UExprTextBlock* SubtitleLineText;

	UPROPERTY(Transient, meta = (BindWidget))
		class UTextBlock* SubtitleNameText;
};
