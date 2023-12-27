// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCEyeShutWidget.h"

UGCEyeShutWidget::UGCEyeShutWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	ShutEyesAnim = nullptr;
}

void UGCEyeShutWidget::SetShutEyesState(const bool bInState)
{
	if (bInState)
	{
		PlayAnimationForward(ShutEyesAnim);
	}
	else
	{
		PlayAnimationReverse(ShutEyesAnim);
	}
}
