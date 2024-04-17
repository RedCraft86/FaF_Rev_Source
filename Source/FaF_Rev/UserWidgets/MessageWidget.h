// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "MessageWidget.generated.h"

class UExprTextBlock;
class UTextBlock;

UCLASS(Abstract)
class FAF_REV_API UMessageWidget final : public UGTUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Elements", meta = (BindWidget))
		UExprTextBlock* SmallNoticeLabel;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Elements", meta = (BindWidget))
		UExprTextBlock* LargeNoticeLabel;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Elements", meta = (BindWidget))
		UExprTextBlock* SubtitleLineText;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Elements", meta = (BindWidget))
		UTextBlock* SubtitleNameText;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Elements", meta = (BindWidget))
		UTextBlock* SubtitleTitleText;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animations", meta = (BindWidget))
		UWidgetAnimation* SmallNoticeAnim;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animations", meta = (BindWidget))
		UWidgetAnimation* LargeNoticeAnim;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animations", meta = (BindWidget))
		UWidgetAnimation* SubtitleAnim;
};
