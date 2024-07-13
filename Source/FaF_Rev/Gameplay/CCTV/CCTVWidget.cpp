// Copyright (C) RedCraft86. All Rights Reserved.

#include "CCTVWidget.h"
#include "CCTVCamera.h"
#include "Components/TextBlock.h"

void UCCTVWidget::UpdateData() const
{
	if (ActiveCamera)
	{
		DisplayName->SetText(ActiveCamera->DisplayName);
		TrackingState->SetText(ActiveCamera->SeesEnemies() ? INVTEXT("Yes") : INVTEXT("No"));
		TrackingState->SetColorAndOpacity(ActiveCamera->SeesEnemies() ? FColor::Green : FColor::Red);
		ConnectState->SetText(ActiveCamera->IsEnabled() ? INVTEXT("Active") : INVTEXT("Reconnecting..."));
	}
	else
	{
		ConnectState->SetText(INVTEXT("Reconnecting..."));
		TrackingState->SetColorAndOpacity(FColor::White);
		TrackingState->SetText(INVTEXT("Unknown"));
	}
}

void UCCTVWidget::NativeConstruct()
{
	Super::NativeConstruct();
	GetWorld()->GetTimerManager().SetTimer(UpdateHandle, this, &UCCTVWidget::UpdateData, 0.5f, true);
}
