// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Quest.h"
#include "Dialogue.h"
#include "MessagingData.h"
#include "NarrativeDataTask.h"
#include "WorldEvents/WorldEvents.h"
#include "FRWorldEvents.generated.h"

USTRUCT(BlueprintType, DisplayName = "Unlock Transient Key")
struct FAF_REV_API FWEUnlockTransientKey final : public FWorldEventBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UnlockTransientKey")
		FGameplayTag TransientKey;

	FWEUnlockTransientKey() : TransientKey({}) {}
	
protected:

	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Unlock Content Key")
struct FAF_REV_API FWEUnlockContentKey final : public FWorldEventBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UnlockContentKey")
		FGameplayTag ContentKey;

	FWEUnlockContentKey() : ContentKey({}) {}
	
protected:

	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Simple Notice (Small)")
struct FAF_REV_API FWESimpleNoticeSmall final : public FWorldEventBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notice")
		TArray<FSimpleNoticeData> Notices;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notice")
		bool bResetQueue;

	FWESimpleNoticeSmall() : Notices({}), bResetQueue(true) {}
	
protected:

	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Simple Notice (Large)")
struct FAF_REV_API FWESimpleNoticeLarge final : public FWorldEventBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notice")
		TArray<FSimpleNoticeData> Notices;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notice")
		bool bResetQueue;

	FWESimpleNoticeLarge() : Notices({}), bResetQueue(true) {}
	
protected:

	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Simple Subtitle")
struct FAF_REV_API FWESimpleSubtitle final : public FWorldEventBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Subtitle")
		TArray<FSimpleSubtitleData> Subtitles;

	FWESimpleSubtitle() : Subtitles({}) {}
	
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
