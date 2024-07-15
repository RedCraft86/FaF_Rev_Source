// Copyright (C) RedCraft86. All Rights Reserved.

#include "CCTVScreenWidget.h"
#include "CCTVMonitor.h"
#include "Components/TextBlock.h"

void UCCTVScreenWidget::UpdateValues()
{
	if (Monitor && Monitor->IsOnActiveCamera())
	{
		const bool bSeeEnemies = Monitor->IsSeeingEnemies();
		TrackingStatusText->SetText(bSeeEnemies ? INVTEXT("Yes") : INVTEXT("No"));
		TrackingStatusText->SetColorAndOpacity(bSeeEnemies ? FColor::Green : FColor::Red);

		CameraStatusText->SetText(INVTEXT("Online"));
		CameraNameText->SetText(Monitor->GetCameraName());
	}
	else
	{
		TrackingStatusText->SetText(INVTEXT("Unknown"));
		TrackingStatusText->SetColorAndOpacity(FColor::White);

		CameraStatusText->SetText(INVTEXT("Reconnecting..."));
		CameraNameText->SetText(INVTEXT("Fetching..."));
	}
}

void UCCTVScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	GetWorld()->GetTimerManager().SetTimer(UpdateTimer, this, &UCCTVScreenWidget::UpdateValues, 0.5f, true);
}
