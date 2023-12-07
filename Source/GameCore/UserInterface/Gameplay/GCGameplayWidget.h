// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "UserInterface/GCUserWidget.h"
#include "GCGameplayWidget.generated.h"

UCLASS(Abstract, DisplayName = "Gameplay Widget Base")
class GAMECORE_API UGCGameplayWidget : public UGCUserWidget
{
	GENERATED_BODY()
	
public:

	UGCGameplayWidget(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(Transient, meta = (BindWidget))
		class UProgressBar* StaminaBar;

	UPROPERTY(Transient, meta = (BindWidget))
		class UTextBlock* CrosshairText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* StaminaAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* CrosshairAnim;
	
protected:

	UPROPERTY(Transient, BlueprintReadOnly)
		class AGCPlayerCharacter* Player;
	
	bool bCachedStaminaState;
	bool bCachedInteractState;
	FTimerHandle InteractTimer;

	void UpdateInteraction();
	virtual void InitData(AGCPlayerController* Controller) override;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& Geometry, float DeltaTime) override;
};
