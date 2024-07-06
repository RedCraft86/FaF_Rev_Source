// Copyright (C) RedCraft86. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "NarrativeWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "NarrativeDataTask.h"
#include "ExprTextBlock.h"
#include "MessageWidget.h"
#include "FRGameMode.h"
#include "FRPlayer.h"

void UQuestTaskWidgetBase::InitWidget(const UNarrativeTask* Task)
{
	TaskObject = Task;
	if (TaskObject)
	{
		FString TaskDisplaySr = TaskObject->GetTaskDescription().ToString();
		if (!TaskObject->DescriptionOverride.IsEmptyOrWhitespace())
		{
			TaskDisplaySr = TaskObject->DescriptionOverride.ToString();
		}
		
		TaskDisplaySr += TaskObject->RequiredQuantity > 1
			? TEXT(" ") + TaskObject->GetTaskProgressText().ToString()
			: TEXT("");
		
		DisplayText->SetText(FText::FromString(TaskDisplaySr));
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
		RemoveFromParent();
	}
}

void UQuestBranchWidgetBase::InitWidget(const UQuestBranch* Branch)
{
	BranchObject = Branch;
	if (BranchObject)
	{
		DisplayText->SetText(BranchObject->OwningQuest->GetQuestName());
		DisplayText->SetVisibility(BranchObject->OwningQuest->GetQuestName().IsEmptyOrWhitespace() || !bUseTitle ?
			ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);

		bool bShouldBeHidden = true;
		for (const UNarrativeTask* Task : BranchObject->QuestTasks)
		{
			if (Task && !Task->bHidden && !Task->IsComplete())
			{
				UQuestTaskWidgetBase* TaskWidget = CreateWidget<UQuestTaskWidgetBase>(this, TaskWidgetClass);
				TaskWidget->InitWidget(Task);

				bShouldBeHidden = false;
				TaskList->AddChild(TaskWidget);
			}
		}

		if (bShouldBeHidden)
		{
			SetVisibility(ESlateVisibility::Collapsed);
			RemoveFromParent();
		}
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
		RemoveFromParent();
	}
}

UDialogueOptionWidgetBase::UDialogueOptionWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer), DisplayText(nullptr), SelectButton(nullptr)
	, MaxCharCount(0), ParentUI(nullptr), ReplyObject(nullptr)
{
#if WITH_EDITOR
	const ConstructorHelpers::FObjectFinder<UNarrativeDataTask> TaskFinder(
		TEXT("/Narrative/DefaultTasks/PlayDialogueNode.PlayDialogueNode"));
	if (TaskFinder.Succeeded()) ReplyTaskType = TaskFinder.Object;
#endif
}

void UDialogueOptionWidgetBase::OnOptionClicked()
{
	ParentUI->SelectDialogueReply(ReplyObject);
}

void UDialogueOptionWidgetBase::MarkSelected() const
{
	DisplayText->SetColorAndOpacity(SelectedColor);
}

void UDialogueOptionWidgetBase::InitWidget(UNarrativeWidgetBase* NarrativeWidget, UDialogue* Dialogue, UDialogueNode_Player* Reply)
{
	ParentUI = NarrativeWidget;
	ReplyObject = Reply;
	if (ParentUI && ReplyObject)
	{
		DisplayText->SetText(ReplyObject->GetOptionText(Dialogue));

		if (ReplyTaskType && ParentUI->NarrativeComponent->HasCompletedTask(ReplyTaskType, ReplyObject->GetID().ToString(), 1))
		{
			DisplayText->SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f));
		}

		SelectButton->OnClicked.AddDynamic(this, &UDialogueOptionWidgetBase::OnOptionClicked);
		if (DisplayText->GetText().IsEmptyOrWhitespace())
		{
			SetVisibility(ESlateVisibility::Collapsed);
			RemoveFromParent();
		}
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
		RemoveFromParent();
	}
}

UNarrativeWidgetBase::UNarrativeWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), QuestBranchBox(nullptr), DialogueNameText(nullptr), DialogueTitleText(nullptr)
	, DialogueLineText(nullptr), SkipLineButton(nullptr), DialogueReplyBox(nullptr), QuestFadeAnim(nullptr)
	, DialogueFadeAnim(nullptr), RepliesFadeAnim(nullptr), HideFadeAnim(nullptr), bNotifyObjectiveUpdates(false)
	, NarrativeComponent(nullptr), bHideQuests(false), bAutoHidden(false), HideCheckTime(0.0f), PlayerChar(nullptr)
	, WorldSettings(nullptr)
{
	ZOrder = 93;
	bAutoAdd = true;

	DialoguePlayerColor = FLinearColor::Yellow;
	DialogueNPCColor = FLinearColor::White;
	DialogueReplyPadding = {0.0f};
}

void UNarrativeWidgetBase::SetQuestsHidden(const bool bInHidden)
{
	if (bHideQuests != bInHidden && !IsAnimationPlaying(QuestFadeAnim))
	{
		bHideQuests = bInHidden;
		bHideQuests ? PlayAnimationForward(QuestFadeAnim) : PlayAnimationReverse(QuestFadeAnim);
	}
}

void UNarrativeWidgetBase::QuestUpdatedNotify()
{
	if (!bNotifyObjectiveUpdates) return;
	if (UMessageWidgetBase* MsgWidget = GetGameMode<AFRGameModeBase>()->GetWidget<UMessageWidgetBase>())
	{
		MsgWidget->QueueSmallNotice({INVTEXT("Objective Updated"), 1.0f});
	}
}

void UNarrativeWidgetBase::RefreshQuestList(const UQuest* Quest, const UQuestBranch* Branch)
{
	bool bShouldHide = true;
	QuestBranchBox->ClearChildren();
	if (Branch && (!Branch->DestinationState || Branch->DestinationState->StateNodeType != EStateNodeType::Failure))
	{
		if (!Branch->DestinationState || Branch->DestinationState->StateNodeType != EStateNodeType::Failure)
		{
			UQuestBranchWidgetBase* BranchWidget = CreateWidget<UQuestBranchWidgetBase>(this, BranchWidgetClass);
			BranchWidget->InitWidget(Branch);

			QuestBranchBox->AddChild(BranchWidget);
			bShouldHide = false;
		}

		QuestUpdatedNotify();
		bHideQuests = false;
		PlayAnimation(QuestFadeAnim, 0.0f, 1, EUMGSequencePlayMode::Reverse);
	}
	
	if (bShouldHide)
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, [this](){
			SetQuestsHidden(true);
		}, 0.25f, false);
	}
}

void UNarrativeWidgetBase::OnQuestNewState(UQuest* Quest, const UQuestState* NewState)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick([this, NewState]()
	{
		bool bShouldHide = true;
		if (NewState && !NewState->Branches.IsEmpty())
		{
			QuestBranchBox->ClearChildren();
			for (const UQuestBranch* Branch : NewState->Branches)
			{
				if (Branch && !Branch->bHidden)
				{
					UQuestBranchWidgetBase* BranchWidget = CreateWidget<UQuestBranchWidgetBase>(this, BranchWidgetClass);
					BranchWidget->InitWidget(Branch);

					QuestBranchBox->AddChild(BranchWidget);
					bShouldHide = false;
				}
			}

			QuestUpdatedNotify();
			bHideQuests = false;
			PlayAnimation(QuestFadeAnim, 0.0f, 1, EUMGSequencePlayMode::Reverse);
		}

		if (bShouldHide)
		{
			FTimerHandle Handle;
			GetWorld()->GetTimerManager().SetTimer(Handle, [this](){
				SetQuestsHidden(true);
			}, 0.25f, false);
		}
	});
}

void UNarrativeWidgetBase::OnQuestTaskCompleted(const UQuest* Quest, const UNarrativeTask* CompletedTask, const UQuestBranch* Branch)
{
	if (Branch && Branch->DestinationState && Branch->DestinationState->StateNodeType != EStateNodeType::Failure)
	{
		RefreshQuestList(Quest, Branch);
	}
}

void UNarrativeWidgetBase::OnQuestTaskProgressChanged(const UQuest* Quest, const UNarrativeTask* ProgressedTask, const UQuestBranch* Branch, int32 OldProgress, int32 NewProgress)
{
	if (Branch && ProgressedTask && !ProgressedTask->IsComplete() &&
		(!Branch->DestinationState || Branch->DestinationState->StateNodeType != EStateNodeType::Failure))
	{
		RefreshQuestList(Quest, Branch);
	}
}

void UNarrativeWidgetBase::SelectDialogueReply(UDialogueNode_Player* Reply)
{
	NarrativeComponent->SelectDialogueOption(Reply);
	for (const TPair<FName, TObjectPtr<UDialogueOptionWidgetBase>>& DialogueReply : DialogueReplies)
	{
		if (!DialogueReply.Value) continue;
		DialogueReply.Value->SetColorAndOpacity(FLinearColor::Gray);
		DialogueReply.Value->DisplayText->SetColorAndOpacity(FLinearColor::White);
		DialogueReply.Value->SetVisibility(ESlateVisibility::HitTestInvisible);
		if (DialogueReply.Key == Reply->GetID())
		{
			DialogueReply.Value->MarkSelected();
		}
	}

	SkipLineButton->SetIsEnabled(true);
	SkipLineButton->SetRenderOpacity(1.0f);
	PlayAnimation(RepliesFadeAnim);
}

void UNarrativeWidgetBase::OnDialogueBegan(UDialogue* Dialogue)
{
	PlayerChar->AddLockFlag(Player::LockFlags::Dialogue);
	CachedInputMode = GetGameMode<AFRGameModeBase>()->GetInputModeData();
	GetGameMode<AFRGameModeBase>()->SetGameInputMode(EGameInputMode::GameAndUI, true,
		EMouseLockMode::LockAlways, false, this);
}

void UNarrativeWidgetBase::OnDialogueFinished(UDialogue* Dialogue, const bool bStartingNewDialogue)
{
	PlayerChar->ClearLockFlag(Player::LockFlags::Dialogue);
	GetGameMode<AFRGameModeBase>()->SetInputModeData(CachedInputMode);
	
	PlayAnimationReverse(DialogueFadeAnim);
	if (GetAnimationCurrentTime(RepliesFadeAnim) > 0.0f)
	{
		PlayAnimationReverse(RepliesFadeAnim);
	}
}

void UNarrativeWidgetBase::OnDialogueRepliesAvailable(UDialogue* Dialogue, const TArray<UDialogueNode_Player*>& PlayerReplies)
{
	DialogueReplyBox->ClearChildren();
	if (PlayerReplies.Num() == 1)
	{
		NarrativeComponent->SelectDialogueOption(PlayerReplies[0]);
		return;
	}

	SkipLineButton->SetIsEnabled(false);
	SkipLineButton->SetRenderOpacity(0.5f);
	for (UDialogueNode_Player* Reply : PlayerReplies)
	{
		if (!Reply) continue;
		UDialogueOptionWidgetBase* Widget = CreateWidget<UDialogueOptionWidgetBase>(this, ReplyWidgetClass);
		Widget->SetPadding(DialogueReplyPadding);
		Widget->InitWidget(this, Dialogue, Reply);

		DialogueReplies.Add(Reply->GetID(), Widget);
		DialogueReplyBox->AddChild(Widget);
	}
	
	PlayAnimation(RepliesFadeAnim);
}

void UNarrativeWidgetBase::OnDialoguePlayerLineStarted(UDialogue* Dialogue, UDialogueNode_Player* Node, const FDialogueLine& DialogueLine)
{
	DialogueNameText->SetText(INVTEXT("You"));
	DialogueNameText->SetColorAndOpacity(DialoguePlayerColor);
	DialogueTitleText->SetVisibility(ESlateVisibility::Collapsed);
	DialogueLineText->SetText(DialogueLine.Text);
	PlayAnimation(DialogueFadeAnim);
}

void UNarrativeWidgetBase::OnDialoguePlayerLineFinished(UDialogue* Dialogue, UDialogueNode_Player* Node, const FDialogueLine& DialogueLine)
{
	PlayAnimationReverse(RepliesFadeAnim);
}

void UNarrativeWidgetBase::OnDialogueNPCLineStarted(UDialogue* Dialogue, UDialogueNode_NPC* Node, const FDialogueLine& DialogueLine, const FSpeakerInfo& Speaker)
{
	DialogueNameText->SetText(FText::FromName(Speaker.SpeakerID));
	DialogueNameText->SetColorAndOpacity(DialogueNPCColor);

	DialogueTitleText->SetText(Speaker.SpeakerName);
	DialogueTitleText->SetVisibility(Speaker.SpeakerName.IsEmptyOrWhitespace()
		? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);

	DialogueLineText->SetText(DialogueLine.Text);
	PlayAnimation(DialogueFadeAnim);
}

void UNarrativeWidgetBase::OnLineSkipClicked()
{
	NarrativeComponent->CurrentDialogue->SkipCurrentLine();
}

void UNarrativeWidgetBase::HideCheck()
{
	HideCheckTime = 0.0f;
	if (!PlayerChar || !WorldSettings || IsAnimationPlaying(HideFadeAnim)) return;
	const bool bNewAutoHidden = PlayerChar->GetActiveCutscene() || WorldSettings->GetPauserPlayerState();
	if (bAutoHidden != bNewAutoHidden)
	{
		bAutoHidden = bNewAutoHidden;
		bAutoHidden ? PlayAnimationForward(HideFadeAnim) : PlayAnimationReverse(HideFadeAnim);
	}
}

void UNarrativeWidgetBase::InitWidget()
{
	NarrativeComponent = GetGameMode<AFRGameModeBase>()->Narrative;
	WorldSettings = GetWorld()->GetWorldSettings();
	PlayerChar = FRPlayer(this);

	NarrativeComponent->OnQuestNewState.AddDynamic(this, &UNarrativeWidgetBase::OnQuestNewState);
	NarrativeComponent->OnQuestTaskCompleted.AddDynamic(this, &UNarrativeWidgetBase::OnQuestTaskCompleted);
	NarrativeComponent->OnQuestTaskProgressChanged.AddDynamic(this, &UNarrativeWidgetBase::OnQuestTaskProgressChanged);

	NarrativeComponent->OnDialogueBegan.AddDynamic(this, &UNarrativeWidgetBase::OnDialogueBegan);
	NarrativeComponent->OnDialogueFinished.AddDynamic(this, &UNarrativeWidgetBase::OnDialogueFinished);
	NarrativeComponent->OnDialogueRepliesAvailable.AddDynamic(this, &UNarrativeWidgetBase::OnDialogueRepliesAvailable);
	NarrativeComponent->OnPlayerDialogueLineStarted.AddDynamic(this, &UNarrativeWidgetBase::OnDialoguePlayerLineStarted);
	NarrativeComponent->OnPlayerDialogueLineFinished.AddDynamic(this, &UNarrativeWidgetBase::OnDialoguePlayerLineFinished);
	NarrativeComponent->OnNPCDialogueLineStarted.AddDynamic(this, &UNarrativeWidgetBase::OnDialogueNPCLineStarted);

	SkipLineButton->OnClicked.AddDynamic(this, &UNarrativeWidgetBase::OnLineSkipClicked);
}

void UNarrativeWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	if (!FRSettings->IsGameplayMap(this))
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UNarrativeWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (HideCheckTime >= 0.25f) { HideCheck(); }
	else { HideCheckTime += InDeltaTime; }
}
