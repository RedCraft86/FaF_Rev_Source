// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "InputModeData.h"
#include "MessagingData.h"
#include "GuideWidget.generated.h"

UCLASS(Abstract)
class FAF_REV_API UGuideWidgetBase final : public UGTUserWidget
{
	GENERATED_BODY()

public:

	UGuideWidgetBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UButton> NextButton;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UWidgetSwitcher> TypeSwitch;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UPanelWidget> CustomPageContainer;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UTextBlock> LocalPageTitle;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UExprTextBlock> LocalPageText;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UImage> LocalPageImage;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class USizeBox> LocalImageContainer;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> GuideFadeAnim;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> NextButtonAnim;

	UPROPERTY(EditAnywhere, Category = "Settings")
		float ImageHeight;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		TObjectPtr<UTexture2D> DefaultImage;
	
	void QueuePage(const FGuideBookPageID& PageID);
	void QueuePages(const TArray<FGuideBookPageID>& PageIDs);

private:

	bool bActive, bPrePauseState;
	FGameInputModeData PreInputMode;
	TQueue<FGuideBookPageID> PageQueue;
	
	void ResetBook() const;
	void ProceedNextGuide();
	UFUNCTION() void OnNextClicked();
	
	virtual void InitWidget() override;
	virtual void NativeConstruct() override;
};
