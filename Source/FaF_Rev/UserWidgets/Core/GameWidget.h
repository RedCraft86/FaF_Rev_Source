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
		TObjectPtr<class UExprTextBlock> InteractLabel;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UProgressBar> StaminaBar;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> HideFadeAnim;

	UPROPERTY(EditAnywhere, Category = "GameWidget")
		TObjectPtr<UTexture2D> DefaultInteractIcon;

	UPROPERTY(EditAnywhere, Category = "GameWidget", meta = (AllowPreserveRatio = true, ClampMin = 0.1f, UIMin = 0.1f))
		FVector2D DefaultInteractSize;

	UPROPERTY(EditAnywhere, Category = "GameWidget", meta = (ClampMin = 0.05f, UIMin = 0.05f))
		FVector2D StaminaBarSpeed;

	UFUNCTION(BlueprintImplementableEvent)
		void OffsetInteractLabel(UExprTextBlock* Label, const FVector2D& BaseOffset, const float IconSizeRatio) const;
	
protected:

	bool bInitFade;
	bool bAutoHidden;
	float HideCheckTime;
	FTimerHandle InteractCheckHandle;
	UPROPERTY(Transient) TObjectPtr<const AWorldSettings> WorldSettings;
	UPROPERTY(Transient) TObjectPtr<class AFRPlayerBase> PlayerChar;

	void HideCheck();
	void InteractCheck() const;
	
	virtual void InitWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
