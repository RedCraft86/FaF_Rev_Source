// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "GameWidget.generated.h"

class UExprTextBlock;

UCLASS(Abstract)
class FAF_REV_API UGameWidget final : public UGTUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Elements|SmallNotice", meta = (BindWidget))
		UExprTextBlock* SmallNoticeLabel;
		
};
