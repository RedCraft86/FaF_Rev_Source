// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "Core/GCSettings.h"
#include "Asset/ExpressiveTextFields.h"
#include "Styles/ExpressiveTextStyleBase.h"
#include "Handles/ExpressiveTextSelector.h"
#include "Inventory/GCInventoryTypes.h"
#include "UserInterface/GCUserWidget.h"
#include "GCMessageWidget.generated.h"

class UImage;
class UTextBlock;

USTRUCT(BlueprintType, DisplayName = "Notice Data")
struct GAMECORE_API FGCNoticeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		bool bUseBasicText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (MultiLine = true, DisplayName = "Text", EditCondition = "bUseBasicText", EditConditionHides))
		FText TextBasic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (EditCondition = "!bUseBasicText", EditConditionHides))
		FExpressiveTextFields Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (ClampMin = 1.0f, UIMin = 1.0f))
		float DisplayTime;

	FGCNoticeData() : bUseBasicText(true), TextBasic(FText::GetEmpty()), DisplayTime(1.0f)
	{
		TextBasic = FText::FromString("Sample Text");
		Text.Text = FText::FromString("Sample Text");
		Text.Alignment = {EExpressiveTextVerticalAlignment::Top, EExpressiveTextHorizontalAlignment::Center};
		Text.Justification = ETextJustify::Center;
		Text.UseDefaultFontSize = true;
		Text.DefaultFontSize = 24;

#if WITH_EDITOR
		if (const TSoftObjectPtr<UExpressiveTextStyleBase> Object = UGCSettings::Get()->BaseTextStyle; !Object.IsNull())
		{
			Text.DefaultStyle = Object.LoadSynchronous();
		}
#endif
	}

	explicit FGCNoticeData(const FText& InText, const float InDisplayTime = 1.0f) : bUseBasicText(false), DisplayTime(InDisplayTime)
	{
		TextBasic = InText;
		Text.Text = InText;
		Text.Alignment = {EExpressiveTextVerticalAlignment::Top, EExpressiveTextHorizontalAlignment::Center};
		Text.Justification = ETextJustify::Center;
		Text.UseDefaultFontSize = true;
		Text.DefaultFontSize = 24;
		
#if WITH_EDITOR
		if (const TSoftObjectPtr<UExpressiveTextStyleBase> Object = UGCSettings::Get()->BaseTextStyle; !Object.IsNull())
		{
			Text.DefaultStyle = Object.LoadSynchronous();
		}
#endif
	}
	
	float GetDisplayTime() const { return FMath::Max(DisplayTime, 1.0f); }
	FExpressiveTextSelector GetText() const
	{
		FExpressiveTextSelector RetVal;
		RetVal.InlinedExpressiveText = Text;
		if (!RetVal.InlinedExpressiveText.DefaultStyle)
		{
			RetVal.InlinedExpressiveText.DefaultStyle = UGCSettings::Get()->BaseTextStyle.LoadSynchronous();
		}
		if (bUseBasicText)
		{
			RetVal.InlinedExpressiveText.Text = TextBasic;
		}
		return RetVal;
	}

	FString GetTextString() const
	{
		return bUseBasicText ? TextBasic.ToString() : Text.Text.ToString();
	}
};

USTRUCT(BlueprintType, DisplayName = "Subtitle Data")
struct GAMECORE_API FGCSubtitleData : public FGCNoticeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		FText Name;

	FGCSubtitleData() : Name(FText::FromString(TEXT("You"))) {}
	
	float GetDisplayTime() const { return FMath::Max(DisplayTime, 1.0f); }
};

UCLASS(Abstract, DisplayName = "Message Widget Base")
class GAMECORE_API UGCMessageWidget : public UGCUserWidget
{
	GENERATED_BODY()
	
public:

	UGCMessageWidget(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* SubtitleName;

	UPROPERTY(Transient, meta = (BindWidget))
		UImage* AchievementIcon;

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* AchievementName;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* NoticeAnim;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* SubtitleAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* AchievementAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MessageWidget")
		UTexture2D* DefaultAchievementIcon;

	UFUNCTION(BlueprintImplementableEvent, meta = (ForceAsFunction = true))
		void SetNoticeText(const FExpressiveTextSelector& InText);

	UFUNCTION(BlueprintImplementableEvent, meta = (ForceAsFunction = true))
		void SetSubtitleText(const FExpressiveTextSelector& InText);

	UFUNCTION(BlueprintCallable, Category = "MessageWidget")
		void QueueNoticeText(const FText InText, const float InTime = 1.0f, const bool bOverrideQueue = false);
	
	UFUNCTION(BlueprintCallable, Category = "MessageWidget")
		void QueueNotice(const FGCNoticeData& InData, const bool bOverrideQueue = false);

	UFUNCTION(BlueprintCallable, Category = "MessageWidget")
		void QueueSubtitleText(const FText InText, const float InTime = 1.0f, const bool bOverrideQueue = false);
	
	UFUNCTION(BlueprintCallable, Category = "MessageWidget")
		void QueueSubtitle(const FGCSubtitleData& InData, const bool bOverrideQueue = false);

	void QueueNoticeByObject(const FGCNoticeData& InData, const UObject* InObject);
	void QueueSubtitleByObject(const FGCSubtitleData& InData, const UObject* InObject);
	
protected:

	UPROPERTY(Transient, BlueprintReadOnly)
		class UGCAchievementManager* AchievementManager;

	UPROPERTY(Transient, BlueprintReadOnly)
		class UGCInventoryManager* InventoryManager;

	TWeakObjectPtr<const UObject> LastNoticeObject;
	TWeakObjectPtr<const UObject> LastSubtitleObject;
	
	TQueue<FGCNoticeData> NoticeQueue;
	TQueue<FGCSubtitleData> SubtitleQueue;
	FGCNoticeData Notice;
	FGCSubtitleData Subtitle;
	FTimerHandle NoticeTimer;
	FTimerHandle SubtitleTimer;
	
	void UpdateNotice();
	void UpdateSubtitle();
	void OnAchievementUnlocked(const struct FGCAchievementID& ID);
	void OnInventoryItemOverflow(const FGCInvItemID& ID, const uint8 Overflow);
	void OnInventoryItemMissing(const FGCInvItemID& ID, const uint8 Missing);
	virtual void InitData(AGCPlayerController* Controller) override;
	
	virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
};
