// Copyright (C) RedCraft86. All Rights Reserved.

#include "CCTVScreenWidget.h"
#include "Components/TextBlock.h"
#include "CCTVMonitor.h"

void UCCTVScreenWidget::UpdateValues() const
{
	if (Monitor && Monitor->IsEnabled() && Monitor->IsOnActiveCamera())
	{
		const bool bSeeEnemies = Monitor->IsSeeingEnemies();
		TrackingStatusText->SetText(bSeeEnemies ? INVTEXT("Detected") : INVTEXT("Not Detected"));
		TrackingStatusText->SetColorAndOpacity(bSeeEnemies ? FColor::Green : FColor::Red);

		CameraStatusText->SetText(INVTEXT("Online"));
		CameraStatusText->SetColorAndOpacity(FColor::Green);
		
		CameraNameText->SetText(Monitor->GetCameraName());
	}
	else
	{
		TrackingStatusText->SetText(INVTEXT("Unknown"));
		TrackingStatusText->SetColorAndOpacity(FColor::White);

		CameraStatusText->SetText(Monitor->IsEnabled() ? INVTEXT("Reconnecting...") : INVTEXT("Offline"));
		CameraStatusText->SetColorAndOpacity(Monitor->IsEnabled() ? FColor::Yellow : FColor::Red);
		
		CameraNameText->SetText(Monitor->IsEnabled() ? INVTEXT("Fetching...") : INVTEXT("None"));
	}
}

void UCCTVScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	GetWorld()->GetTimerManager().SetTimer(UpdateTimer, this, &UCCTVScreenWidget::UpdateValues, 0.25f, true);
}
