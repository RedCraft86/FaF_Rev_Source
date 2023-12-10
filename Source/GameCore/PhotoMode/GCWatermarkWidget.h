// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "GCWatermarkWidget.generated.h"

UCLASS(Abstract, DisplayName = "Watermark Widget Base")
class GAMECORE_API UGCWatermarkWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UGCWatermarkWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		class UImage* BackgroundImage;

	UFUNCTION(BlueprintImplementableEvent)
		void AdjustWatermark(const FVector2D CaptureSize);

	class UCanvasRenderTarget2D* ProcessImage(UTexture2D* InTexture);
};