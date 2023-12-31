﻿// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "Core/CoreMacros.h"
#include "Engine/DataTable.h"
#include "Core/GCSettings.h"
#include "UserInterface/GCUserWidget.h"
#include "Asset/ExpressiveTextFields.h"
#include "Styles/ExpressiveTextStyleBase.h"
#include "Handles/ExpressiveTextSelector.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "GCInfoBookWidget.generated.h"

USTRUCT(BlueprintType, DisplayName = "Info Page ID")
struct GAMECORE_API FGCInfoPageID : public FPulldownStructBase
{
	GENERATED_BODY()
	SETUP_PULLDOWN(FGCInfoPageID);
};
SETUP_PULLDOWN_HASH(FGCInfoPageID);

USTRUCT(BlueprintType, DisplayName = "Info Page Data")
struct GAMECORE_API FGCInfoPageData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InfoPageData")
		FText Label;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InfoPageData", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		bool bShowBackground;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InfoPageData", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		float MinWaitTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InfoPageData")
		TSubclassOf<UUserWidget> Widget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InfoPageData|Override", meta = (EditCondition = "Widget == nullptr"))
		FExpressiveTextFields Message;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InfoPageData|Override", meta = (EditCondition = "Widget == nullptr"))
		TSoftObjectPtr<UTexture2D> Image;
	
	FGCInfoPageData()
		: Label(FText::FromString("Placeholder"))
		, bShowBackground(false)
		, MinWaitTime(1.0f)
		, Image(nullptr)
	{
		Message.Text = FText::FromString("Placeholder");
		Message.Alignment = {EExpressiveTextVerticalAlignment::Top, EExpressiveTextHorizontalAlignment::Center};
		Message.Justification = ETextJustify::Center;
		Message.UseDefaultFontSize = true;
		Message.DefaultFontSize = 24;

#if WITH_EDITOR
		if (const TSoftObjectPtr<UExpressiveTextStyleBase> Object = UGCSettings::Get()->BaseTextStyle; !Object.IsNull())
		{
			Message.DefaultStyle = Object.LoadSynchronous();
		}
#endif
	}

	bool HasEmptyMessage() const { return Message.Text.IsEmptyOrWhitespace(); }
	FExpressiveTextSelector GetMessage() const
	{
		FExpressiveTextSelector RetVal;
		RetVal.InlinedExpressiveText = Message;
		if (!RetVal.InlinedExpressiveText.DefaultStyle)
		{
			RetVal.InlinedExpressiveText.DefaultStyle = UGCSettings::Get()->BaseTextStyle.LoadSynchronous();
		}
		return RetVal;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInfoBookExitedBP);

UCLASS(Abstract, DisplayName = "Info Book Widget Base")
class GAMECORE_API UGCInfoBookWidget : public UGCUserWidget
{
	GENERATED_BODY()

public:

	UGCInfoBookWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, DisplayName = "On Info Book Exited")
		FOnInfoBookExitedBP OnInfoBookExitedBP;
	
	UPROPERTY(Transient, meta = (BindWidget))
		class UButton* ContinueButton;
	
	UPROPERTY(Transient, meta = (BindWidget))
		class UTextBlock* LabelText;

	UPROPERTY(Transient, meta = (BindWidget))
		UPanelWidget* WidgetBox;
	
	UPROPERTY(Transient, meta = (BindWidget))
		class USizeBox* MessageBox;

	UPROPERTY(Transient, meta = (BindWidget))
		class UImage* ContentImage;

	UPROPERTY(Transient, meta = (BindWidget))
		UPanelWidget* ContentBox;

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* ContinueText;

	UPROPERTY(Transient, meta = (BindWidget))
		UImage* BackgroundContent;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* ContentFadeAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* ContinueFadeAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InfoBookWidget")
		float ImageMsgHeight;

	UFUNCTION(BlueprintImplementableEvent, meta = (ForceAsFunction = true))
		void SetMessageText(const FExpressiveTextSelector& InText, const ESlateVisibility& InVisibility);
	
	UFUNCTION(BlueprintCallable, Category = "InfoBookWidget")
		void QueuePage(const FGCInfoPageID PageID);
	
	UFUNCTION(BlueprintCallable, Category = "InfoBookWidget", meta = (AutoCreateRefTerm = "PageIDs"))
		void QueuePages(const TArray<FGCInfoPageID>& PageIDs);

	DECLARE_MULTICAST_DELEGATE(FOnExited)
	FOnExited OnInfoBookExited;
	
protected:

	UPROPERTY(Transient)
		UDataTable* PageTable;

	UPROPERTY(Transient)
		UUserWidget* ActiveWidget;
	
	FName CurrentPage;
	TQueue<FName> Pages;
	float ContinueWaitTime;
	
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& InGeometry, float InDeltaTime) override;
	virtual void RemoveWidget(const TFunction<void()>& OnFinished, const bool bFade) override;

	UFUNCTION()
		void OnContinueClicked();
};
