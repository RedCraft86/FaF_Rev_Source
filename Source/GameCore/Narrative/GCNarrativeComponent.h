// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "NarrativeComponent.h"
#include "GCNarrativeComponent.generated.h"

UCLASS(DisplayName = "Extended Narrative Component", NotBlueprintable, ClassGroup = (Gameplay), meta = (BlueprintSpawnableComponent))
class GAMECORE_API UGCNarrativeComponent final : public UNarrativeComponent
{
	GENERATED_BODY()

public:

	static UGCNarrativeComponent* Get(const UObject* WorldContext);
	
	virtual void QuestNewState(UQuest* Quest, const UQuestState* State) override;
	virtual void QuestBranchCompleted(const UQuest* Quest, const UQuestBranch* Branch) override;
	virtual void QuestTaskCompleted(const UQuest* Quest, const UNarrativeTask* Task, const UQuestBranch* Branch) override;
	virtual void QuestTaskProgressMade(const UQuest* Quest, const UNarrativeTask* Task, const UQuestBranch* Branch, int32 Current, int32 Target) override;

	FString GetLastQuestData() const;

private:

	const UQuest* LastQuest = nullptr;
	const UQuestBranch* LastBranch = nullptr;
};
