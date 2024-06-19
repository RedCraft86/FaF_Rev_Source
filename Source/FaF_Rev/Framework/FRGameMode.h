// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTGameMode.h"
#include "MessagingData.h"
#include "FRGameMode.generated.h"

#define FRGameMode(Context) AGTGameModeBase::Get<AFRGameModeBase>(Context)

UCLASS(Abstract)
class FAF_REV_API AFRGameModeBase final : public AGTGameModeBase
{
	GENERATED_BODY()

public:

	AFRGameModeBase();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		TObjectPtr<class UNarrativeComponent> Narrative;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		TObjectPtr<class UInventoryComponent> Inventory;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "GameMode")
		TObjectPtr<class AFRPlayerController> PlayerController;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "GameMode")
		TObjectPtr<class AFRPlayerBase> PlayerCharacter;

private:

	virtual void BeginPlay() override;

public: // Shortcuts

	UFUNCTION(BlueprintCallable, Category = "DisplayMessages")
		void QueueSmallNotice(const FSimpleNoticeData& NoticeData, const bool bResetQueue = false);

	UFUNCTION(BlueprintCallable, Category = "DisplayMessages")
		void QueueLargeNotice(const FSimpleNoticeData& NoticeData, const bool bResetQueue = false);

	UFUNCTION(BlueprintCallable, Category = "DisplayMessages")
		void QueueSubtitle(const FSimpleSubtitleData& SubtitleData, const bool bOverride = false);
	
	UFUNCTION(BlueprintCallable, Category = "DisplayMessages")
		void QueueSubtitles(const TArray<FSimpleSubtitleData>& Subtitles, const bool bOverride = false);

	UFUNCTION(BlueprintCallable, Category = "DisplayMessages")
		void QueueGuidePage(const FGuideBookPageID PageID);
	
	UFUNCTION(BlueprintCallable, Category = "DisplayMessages")
		void QueueGuidePages(const TArray<FGuideBookPageID>& PageIDs);
};
