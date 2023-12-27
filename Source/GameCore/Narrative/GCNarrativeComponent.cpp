// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCNarrativeComponent.h"
#include "Player/GCPlayerController.h"

UGCNarrativeComponent* UGCNarrativeComponent::Get(const UObject* WorldContext)
{
	const AGCPlayerController* PC = AGCPlayerController::Get(WorldContext);
	return PC ? PC->GetNarrativeComponent() : nullptr;
}

void UGCNarrativeComponent::QuestNewState(UQuest* Quest, const UQuestState* State)
{
	LastQuest = Quest;
	Super::QuestNewState(Quest, State);
}

void UGCNarrativeComponent::QuestBranchCompleted(const UQuest* Quest, const UQuestBranch* Branch)
{
	LastQuest = Quest;
	LastBranch = Branch;
	Super::QuestBranchCompleted(Quest, Branch);
}

void UGCNarrativeComponent::QuestTaskCompleted(const UQuest* Quest, const UNarrativeTask* Task, const UQuestBranch* Branch)
{
	LastQuest = Quest;
	LastBranch = Branch;
	Super::QuestTaskCompleted(Quest, Task, Branch);
}

void UGCNarrativeComponent::QuestTaskProgressMade(const UQuest* Quest, const UNarrativeTask* Task,const UQuestBranch* Branch, int32 Current, int32 Target)
{
	LastQuest = Quest;
	LastBranch = Branch;
	Super::QuestTaskProgressMade(Quest, Task, Branch, Current, Target);
}

FString UGCNarrativeComponent::GetLastQuestData() const
{
	if (LastQuest && LastBranch)
	{
		FString FirstIncompleteTask = TEXT("");
		for (const UNarrativeTask* Task : LastBranch->QuestTasks)
		{
			if (Task && !Task->IsComplete())
			{
				FirstIncompleteTask = Task->DescriptionOverride.IsEmptyOrWhitespace() ?
					Task->GetTaskDescription().ToString() : Task->DescriptionOverride.ToString();
			}
		}
		
		return FString::Printf(TEXT("%s_%s_%s"), *LastQuest->GetClass()->GetName(),
			*LastBranch->Description.ToString(), *FirstIncompleteTask);
	}

	return TEXT("");
}
