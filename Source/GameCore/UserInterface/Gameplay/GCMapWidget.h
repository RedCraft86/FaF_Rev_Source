// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "UserInterface/GCUserWidget.h"
#include "GCMapWidget.generated.h"

UCLASS(Abstract, DisplayName = "Map Widget Base")
class GAMECORE_API UGCMapWidget : public UGCUserWidget
{
	GENERATED_BODY()

public:

	UGCMapWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		class UWidgetSwitcher* MapSwitcher;

	UPROPERTY(Transient, meta = (BindWidget))
		class UButton* ExitButton;
	
	void SetMap(FGameplayTag MapID) const;
	virtual void InitData(AGCPlayerController* Controller) override;

private:
	
	UPROPERTY(EditAnywhere, Category = "MapWidget")
		TArray<FGameplayTag> MapIndexes;

	UPROPERTY()
		void ExitMapWidget();
};
