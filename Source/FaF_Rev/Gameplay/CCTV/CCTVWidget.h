// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CCTVWidget.generated.h"

class UTextBlock;

UCLASS(Abstract)
class FAF_REV_API UCCTVWidget final : public UUserWidget
{
	GENERATED_BODY()

	friend class ACCTVMonitor;

public:

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> DisplayName;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> TrackingState;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> ConnectState;

protected:

	UPROPERTY() FTimerHandle UpdateHandle;
	UPROPERTY(Transient) TObjectPtr<const class ACCTVCamera> ActiveCamera;
	
	void UpdateData() const;
	virtual void NativeConstruct() override;
};
