// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "FaF_Rev.h"
#include "FRSettings.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "MessagingData.generated.h"

USTRUCT(BlueprintType)
struct FAF_REV_API FSimpleNoticeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextData", meta = (MultiLine = true))
		FText Message;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextData")
		float ExtraTime;

	FSimpleNoticeData() : Message(FText::GetEmpty()), ExtraTime(1.0f) {}
	FSimpleNoticeData(const FText& InMessage, const float InTime) : Message(InMessage), ExtraTime(InTime) {}
	friend uint32 GetTypeHash(const FSimpleNoticeData& Data)
	{
		return HashCombine(GetTypeHash(Data.Message.ToString()), GetTypeHash(Data.ExtraTime));
	}

	bool IsValidData() const { return !Message.IsEmptyOrWhitespace(); }
	float CalcDisplayTime() const { return FRSettings->CalcReadingTime(Message.ToString()) + ExtraTime; }
};

USTRUCT(BlueprintType)
struct FAF_REV_API FSimpleSubtitleData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextData")
		FText Speaker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextData", meta = (MultiLine = true))
		FText Line;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubtitleData")
		float ExtraTime;

	FSimpleSubtitleData() : Speaker(INVTEXT("You")), Line(FText::GetEmpty()), ExtraTime(1.0f) {}
	FSimpleSubtitleData(const FText& InLabel,const FText& InLine, const float InTime) : Speaker(InLabel), Line(InLine), ExtraTime(InTime) {}
	friend uint32 GetTypeHash(const FSimpleSubtitleData& Data)
	{
		return HashCombine(HashCombine(GetTypeHash(Data.Speaker.ToString()),
			GetTypeHash(Data.Line.ToString())), GetTypeHash(Data.ExtraTime));
	}

	bool IsValidData() const { return !Speaker.IsEmptyOrWhitespace() && !Line.IsEmptyOrWhitespace(); }
	float CalcDisplayTime() const { return FRSettings->CalcReadingTime(Line.ToString()) + ExtraTime; }
};

USTRUCT(BlueprintType, DisplayName = "Guide Book Page ID")
struct FAF_REV_API FGuideBookPageID : public FPulldownStructBase
{
	GENERATED_BODY()
	SETUP_PULLDOWN(FGuideBookPageID)
};

USTRUCT(BlueprintType)
struct FAF_REV_API FGuideBookPageData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuideBookPageData")
		TSoftClassPtr<UUserWidget> CustomWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuideBookPageData", meta = (EditCondition = "CustomWidget == nullptr"))
		FText Label;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuideBookPageData", meta = (EditCondition = "CustomWidget == nullptr", MultiLine = true))
		FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuideBookPageData", meta = (EditCondition = "CustomWidget == nullptr"))
		TSoftObjectPtr<UTexture2D> Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuideBookPageData")
		float WaitTime;
	
	FGuideBookPageData()
		: CustomWidget(nullptr)
		, Label(INVTEXT("Empty Guide"))
		, Description(FText::GetEmpty())
		, Image(nullptr)
		, WaitTime(1.0f)
	{}

	bool IsValidData() const { return WaitTime > 0.25f && (!CustomWidget.IsNull() || !Label.IsEmptyOrWhitespace()); }
};
