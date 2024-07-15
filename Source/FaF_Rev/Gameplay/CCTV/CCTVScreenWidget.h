// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CCTVScreenWidget.generated.h"

class UTextBlock;

UCLASS(Abstract)
class FAF_REV_API UCCTVScreenWidget final : public UUserWidget
{
	GENERATED_BODY()

	friend class ACCTVMonitor;

public:

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> TrackingStatusText;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> CameraStatusText;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> CameraNameText;

protected:

	UPROPERTY() FTimerHandle UpdateTimer;
	UPROPERTY(Transient) TObjectPtr<const ACCTVMonitor> Monitor;

	void UpdateValues() const;
	virtual void NativeConstruct() override;
};
