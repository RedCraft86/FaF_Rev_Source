// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCMapWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Player/GCPlayerController.h"

UGCMapWidget::UGCMapWidget(const FObjectInitializer& ObjectInitializer)
	: UGCUserWidget(ObjectInitializer)
{
	MapSwitcher = nullptr;
	ExitButton = nullptr;
}

void UGCMapWidget::SetMap(const FGameplayTag MapID) const
{
	MapSwitcher->SetActiveWidgetIndex(FMath::Max(0, MapIndexes.Find(MapID) + 1));
}

void UGCMapWidget::InitData(AGCPlayerController* Controller)
{
	Super::InitData(Controller);
	ExitButton->OnClicked.AddDynamic(this, &UGCMapWidget::ExitMapWidget);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGCMapWidget::ExitMapWidget()
{
	PlayerController->CloseMap();
}
