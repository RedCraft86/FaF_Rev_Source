// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Quest.h"
#include "Dialogue.h"
#include "MessagingData.h"
#include "NarrativeDataTask.h"
#include "WorldEvents/WorldEvents.h"
#include "FRWorldEvents.generated.h"

USTRUCT(BlueprintType, DisplayName = "Game Notice (Small)")
struct FAF_REV_API FWEGameNoticeSmall final : public FWorldEventBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notice")
		TArray<FSimpleNoticeData> Notices;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notice")
		bool bResetQueue;

	FWEGameNoticeSmall() : Notices({}), bResetQueue(true) {}
	
protected:

	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Game Notice (Large)")
struct FAF_REV_API FWEGameNoticeLarge final : public FWorldEventBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notice")
		TArray<FSimpleNoticeData> Notices;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notice")
		bool bResetQueue;

	FWEGameNoticeLarge() : Notices({}), bResetQueue(true) {}
	
protected:

	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Game Subtitle")
struct FAF_REV_API FWEGameSubtitle final : public FWorldEventBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Subtitle")
		TArray<FSimpleSubtitleData> Subtitles;

	FWEGameSubtitle() : Subtitles({}) {}
	
protected:

	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Narrative Dialogue")
struct FAF_REV_API FWENarrativeDialogue final : public FWorldEventBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
		TSoftClassPtr<UDialogue> Dialogue;

	FWENarrativeDialogue() : Dialogue(nullptr) {}
	
protected:

	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Narrative Quest")
struct FAF_REV_API FWENarrativeQuest final : public FWorldEventBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
		TSoftClassPtr<UQuest> Quest;

	FWENarrativeQuest() : Quest(nullptr) {}
	
protected:

	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Narrative Task")
struct FAF_REV_API FWENarrativeTask final : public FWorldEventBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task")
		TSoftObjectPtr<UNarrativeDataTask> Task;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task")
		FString Argument;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task")
		uint8 Quantity;
	
	FWENarrativeTask() : Task(nullptr), Argument(TEXT("")), Quantity(1) {}
	
protected:

	virtual void RunEvent(const UObject* WorldContext) override;
};
