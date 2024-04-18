// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "FaF_Rev.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "MessagingData.generated.h"

USTRUCT(BlueprintType)
struct FAF_REV_API FSimpleNoticeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoticeData", meta = (MultiLine = true))
		FText Message;

	// Additional show time. Added on top of the words per second provided in Game Project settings.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoticeData")
		float DisplayTime;

	FSimpleNoticeData() : Message(FText::GetEmpty()), DisplayTime(0.0f) {}
	FSimpleNoticeData(const FText& InMessage, const float InTime) : Message(InMessage), DisplayTime(InTime) {}
	friend uint32 GetTypeHash(const FSimpleNoticeData& Data)
	{
		return HashCombine(GetTypeHash(Data.Message.ToString()), GetTypeHash(Data.DisplayTime));
	}

	bool IsValidData() const { return !Message.IsEmptyOrWhitespace(); }
};

USTRUCT(BlueprintType)
struct FAF_REV_API FSimpleSubtitleData
{
	GENERATED_BODY()

	// Speaker name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubtitleData")
		FText Label;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubtitleData", meta = (MultiLine = true))
		FText Line;

	// Additional show time. Added on top of the words per second provided in Game Project settings.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubtitleData")
		float DisplayTime;

	FSimpleSubtitleData() : Label(INVTEXT("You")), Line(FText::GetEmpty()), DisplayTime(0.0f) {}
	FSimpleSubtitleData(const FText& InLabel,const FText& InLine, const float InTime) : Label(InLabel), Line(InLine), DisplayTime(InTime) {}
	friend uint32 GetTypeHash(const FSimpleSubtitleData& Data)
	{
		return HashCombine(HashCombine(GetTypeHash(Data.Label.ToString()),
			GetTypeHash(Data.Line.ToString())), GetTypeHash(Data.DisplayTime));
	}

	bool IsValidData() const { return !Label.IsEmptyOrWhitespace() && !Line.IsEmptyOrWhitespace(); }
};

USTRUCT(BlueprintType, DisplayName = "Guide Book Page ID")
struct FAF_REV_API FGuideBookPageID : public FPulldownStructBase
{
	GENERATED_BODY()
	SETUP_PULLDOWN(FGuideBookPageID);
};

USTRUCT(BlueprintType)
struct FAF_REV_API FGuideBookPageData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuideBookPageData")
		TSubclassOf<UUserWidget> CustomWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuideBookPageData", meta = (EditCondition = "CustomWidget == nullptr"))
		FText Label;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuideBookPageData", meta = (EditCondition = "CustomWidget == nullptr", MultiLine = true))
		FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuideBookPageData", meta = (EditCondition = "CustomWidget == nullptr"))
		TSoftObjectPtr<UTexture2D> Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuideBookPageData", meta = (EditCondition = "CustomWidget == nullptr"))
		bool bShowBackground;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuideBookPageData")
		float WaitTime;
	
	FGuideBookPageData()
		: CustomWidget(nullptr)
		, Label(INVTEXT("Empty Guide"))
		, Description(FText::GetEmpty())
		, Image(nullptr)
		, bShowBackground(false)
		, WaitTime(1.0f)
	{}
};
