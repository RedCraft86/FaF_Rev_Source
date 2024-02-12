// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "GCFrameRateWidget.generated.h"

class UTextBlock;

UCLASS(Abstract, DisplayName = "FPS Widget Base")
class GAMECORE_API UGCFrameRateWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UGCFrameRateWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* FrameRateText;

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* DeltaTimeText;

protected:

	float ProgressedTime;
	
	FLinearColor GetFrameRateColor(const float InFrameRate) const;
	virtual void NativeTick(const FGeometry& InGeometry, float DeltaTime) override;
};