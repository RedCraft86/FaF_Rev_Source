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
		UExprTextBlock* SmallNoticeText;

	UPROPERTY(Transient, meta = (BindWidget))
		UExprTextBlock* LargeNoticeText;

	UPROPERTY(Transient, meta = (BindWidget))
		UPanelWidget* SubtitleContainer;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UExprTextBlock* SubtitleLineText;

	UPROPERTY(Transient, meta = (BindWidget))
		class UTextBlock* SubtitleNameText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* SmallNoticeAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* LargeNoticeAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* SubtitleAnim;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* SubtitleHideAnim;
};
