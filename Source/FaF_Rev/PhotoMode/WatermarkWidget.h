// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "WatermarkWidget.generated.h"

UCLASS(Abstract)
class FAF_REV_API UWatermarkWidgetBase final : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(Transient, meta = (BindWidget))
		class UImage* BaseImage;

	void ClearImage();
	UTexture2D* AddWatermark(UTexture2D* InTexture);

protected:

	UPROPERTY(Transient) UTexture2D* ImageTex;
};
