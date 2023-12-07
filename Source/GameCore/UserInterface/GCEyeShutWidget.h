// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "GCEyeShutWidget.generated.h"

UCLASS(Abstract, DisplayName = "Eye Shut Widget Base")
class GAMECORE_API UGCEyeShutWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UGCEyeShutWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* ShutEyesAnim;
	
	void SetShutEyesState(const bool bInState);
};
