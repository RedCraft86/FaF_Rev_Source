// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "MessagingData.h"
#include "GuideWidget.generated.h"

UCLASS(Abstract)
class FAF_REV_API UGuideWidgetBase final : public UGTUserWidget
{
	GENERATED_BODY()

public:

	UGuideWidgetBase(const FObjectInitializer& ObjectInitializer);

	void QueuePage(const FGuideBookPageID& PageID);
	void QueuePages(const TArray<FGuideBookPageID>& PageIDs);

private:

	UPROPERTY(EditAnywhere, Category = "Settings")
		float ImageHeight;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		UTexture2D* DefaultImage;
	
	UPROPERTY(Transient, meta = (BindWidget))
		class UButton* NextButton;
	
	UPROPERTY(Transient, meta = (BindWidget))
		class UWidgetSwitcher* TypeSwitch;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UPanelWidget* CustomPageContainer;
	
	UPROPERTY(Transient, meta = (BindWidget))
		class UTextBlock* LocalPageTitle;
	
	UPROPERTY(Transient, meta = (BindWidget))
		class UExprTextBlock* LocalPageText;
	
	UPROPERTY(Transient, meta = (BindWidget))
		class UImage* LocalPageImage;
	
	UPROPERTY(Transient, meta = (BindWidget))
		class USizeBox* LocalImageContainer;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* GuideFadeAnim;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* NextButtonAnim;

	bool bActive, bPrePauseState;
	TQueue<FGuideBookPageID> PageQueue;
	void ProceedNextGuide();
	void ResetBook() const;

	UFUNCTION() void OnNextClicked();
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
