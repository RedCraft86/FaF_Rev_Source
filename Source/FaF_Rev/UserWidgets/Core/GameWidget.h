// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "GameWidget.generated.h"

class UImage;

UCLASS(Abstract)
class FAF_REV_API UGameWidgetBase final : public UGTUserWidget
{
	GENERATED_BODY()

public:

	UGameWidgetBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UImage> Crosshair;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UImage> InteractIcon;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UTextBlock> InteractLabel;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UProgressBar> StaminaBar;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> HideFadeAnim;

	UPROPERTY(EditAnywhere, Category = "GameWidget")
		TObjectPtr<UTexture2D*> DefaultInteractIcon;

	UPROPERTY(EditAnywhere, Category = "GameWidget")
		float StaminaBarSpeed;

protected:

	float HideCheckTime;
	FTimerHandle InteractCheckHandle;
	UPROPERTY(Transient) TObjectPtr<const AWorldSettings> WorldSettings;
	UPROPERTY(Transient) TObjectPtr<class AFRPlayerBase> PlayerChar;

	void HideCheck();
	void InteractCheck();
	
	virtual void InitWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
