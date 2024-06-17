﻿// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Quest.h"
#include "Dialogue.h"
#include "FRPlayer.h"
#include "MessagingData.h"
#include "NarrativeDataTask.h"
#include "Data/PulldownTypes.h"
#include "WorldEvents/WorldEvents.h"
#include "FRWorldEvents.generated.h"

USTRUCT(BlueprintType, DisplayName = "Change Player Settings")
struct FAF_REV_API FWEPlayerSettings final : public FWorldEventBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerSettings")
		FPlayerSettings Settings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerSettings")
		TSoftObjectPtr<AFRPlayerBase> Player;

	FWEPlayerSettings() : Settings({}) {}
	
protected:

	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Player Lock Flag")
struct FAF_REV_API FWEPlayerLockFlag final : public FWorldEventBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerLockFlag")
		bool bClearFlag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerLockFlag")
		FPlayerLockFlag LockFlag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerLockFlag")
		TSoftObjectPtr<AFRPlayerBase> Player;

	FWEPlayerLockFlag() : bClearFlag(false), LockFlag(Player::LockFlags::Custom) {}
	
protected:

	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Player Fade")
struct FAF_REV_API FWEPlayerFade final : public FWorldEventBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerFade")
		bool bFadeOut;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerFade")
		bool bFadeAudio;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerFade", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float FadeTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerFade")
		TSoftObjectPtr<AFRPlayerBase> Player;

	FWEPlayerFade() : bFadeOut(false), bFadeAudio(false), FadeTime(1.0f) {}
	
protected:

	virtual void RunEvent(const UObject* WorldContext) override;
};

USTRUCT(BlueprintType, DisplayName = "Player Lock On")
struct FAF_REV_API FWEPlayerLockOn final : public FWorldEventBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerLockOn")
		float LockOnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerLockOn")
		TSoftObjectPtr<AActor> Target;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerLockOn")
		FStringListPulldown Component;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerLockOn")
		TSoftObjectPtr<AFRPlayerBase> Player;

	FWEPlayerLockOn() : LockOnSpeed(5.0f) {}
	
protected:

	virtual void RunEvent(const UObject* WorldContext) override;
#if WITH_EDITORONLY_DATA
	virtual void OnConstruction(const UObject* WorldContext, const bool bEditorTime) override;
#endif
};

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

USTRUCT(BlueprintType, DisplayName = "Guide Pages")
struct FAF_REV_API FWESimpleGuide final : public FWorldEventBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuidePages")
		TArray<FGuideBookPageID> Pages;

	FWESimpleGuide() : Pages({}) {}
	
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

USTRUCT(BlueprintType, DisplayName = "Step into Sequence")
struct FAF_REV_API FWEStepSequence final : public FWorldEventBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StepSequence")
		TSoftObjectPtr<AFRPlayerBase> Player;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StepSequence")
		uint8 Index;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StepSequence", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float FadeTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StepSequence")
		bool bLock;

	FWEStepSequence() : Index(0), FadeTime(1.0f), bLock(true) {}

protected:

	virtual void RunEvent(const UObject* WorldContext) override;
};
