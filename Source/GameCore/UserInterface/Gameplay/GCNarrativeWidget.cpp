// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "GCNarrativeWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Components/RichTextBlock.h"
#include "Animation/UMGSequencePlayer.h"
#include "Narrative/GCNarrativeComponent.h"
#include "UserInterface/Messaging/GCMessageWidget.h"
#include "Player/GCPlayerController.h"
#include "NarrativeDataTask.h"
#include "Core/GCSettings.h"
#if WITH_EDITOR
#include "UObject/ConstructorHelpers.h"
#endif

UGCQuestTaskWidget::UGCQuestTaskWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	DisplayText = nullptr;
	TaskObject = nullptr;
}

void UGCQuestTaskWidget::InitWidget(const UNarrativeTask* Task)
{
	TaskObject = Task;
	if (TaskObject)
	{
		FString TaskDisplaySr = TaskObject->GetTaskDescription().ToString();
		if (!TaskObject->DescriptionOverride.IsEmptyOrWhitespace())
		{
			TaskDisplaySr = TaskObject->DescriptionOverride.ToString();
		}

		const FString CountDisplay(FString::Printf(TEXT(" (%d/%d)"),
			TaskObject->GetProgress(), TaskObject->RequiredQuantity));
		
		TaskDisplaySr += TaskObject->bOptional ? TEXT(" (Optional)") : TEXT("");
		TaskDisplaySr += TaskObject->RequiredQuantity > 1 ? CountDisplay : TEXT("");
		
		DisplayText->SetText(FText::FromString(TaskDisplaySr));
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
		RemoveFromParent();
	}
}

UGCQuestBranchWidget::UGCQuestBranchWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	DisplayText = nullptr;
	NormalTaskBox = nullptr;
	OptionalTaskBox = nullptr;
	TaskWidgetClass = NULL;
	BranchObject = nullptr;
}

void UGCQuestBranchWidget::InitWidget(const UQuestBranch* Branch)
{
	BranchObject = Branch;
	if (BranchObject)
	{
		DisplayText->SetText(BranchObject->OwningQuest->QuestName);
		DisplayText->SetVisibility(BranchObject->OwningQuest->QuestName.IsEmptyOrWhitespace() ?
			ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);

		bool bShouldBeHidden = true;
		for (const UNarrativeTask* Task : BranchObject->QuestTasks)
		{
			if (Task && !Task->bHidden && Task->GetProgress() < Task->RequiredQuantity)
			{
				UGCQuestTaskWidget* TaskWidget = CreateWidget<UGCQuestTaskWidget>(GetOwningPlayer(), TaskWidgetClass);
				TaskWidget->InitWidget(Task);

				if (Task->bOptional)
				{
					OptionalTaskBox->AddChild(TaskWidget);
				}
				else
				{
					bShouldBeHidden = false;
					NormalTaskBox->AddChild(TaskWidget);
				}
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

UGCDialogueOptionWidget::UGCDialogueOptionWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	DisplayText = nullptr;
	SelectButton = nullptr;
	MaxCharCount = 15;
	SelectedColor = FLinearColor::Yellow;
	ReplyTaskType = nullptr;
	
	ParentUI = nullptr;
	ReplyObject = nullptr;

#if WITH_EDITOR
	const ConstructorHelpers::FObjectFinder<UNarrativeDataTask> TaskFinder(TEXT("/Narrative/DefaultTasks/PlayDialogueNode.PlayDialogueNode"));
	ReplyTaskType = TaskFinder.Object;
#endif
}

void UGCDialogueOptionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SelectButton->OnClicked.AddDynamic(this, &UGCDialogueOptionWidget::OnOptionClicked);
}

void UGCDialogueOptionWidget::InitWidget(UGCNarrativeWidget* NarrativeWidget, UDialogue* Dialogue, UDialogueNode_Player* Reply)
{
	ParentUI = NarrativeWidget;
	ReplyObject = Reply;
	if (ParentUI && ReplyObject)
	{
		const FString RawDesc = ReplyObject->GetOptionText(Dialogue).ToString();
		DisplayText->SetText(FText::FromString(RawDesc.Len() > MaxCharCount ? RawDesc.Mid(0, MaxCharCount) + TEXT("...") : RawDesc));

		if (ReplyTaskType && ParentUI->GetNarrative()->HasCompletedTask(ReplyTaskType, ReplyObject->GetID().ToString(), 1))
		{
			DisplayText->SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f));
		}
		
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

void UGCDialogueOptionWidget::MarkSelected() const
{
	DisplayText->SetColorAndOpacity(SelectedColor);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGCDialogueOptionWidget::OnOptionClicked()
{
	ParentUI->SelectDialogueReply(ReplyObject);
}

UGCNarrativeWidget::UGCNarrativeWidget(const FObjectInitializer& ObjectInitializer)
	: UGCUserWidget(ObjectInitializer)
{
	WidgetZOrder = 11;
	bAutoAddToViewport = true;

	QuestBox = nullptr;
	QuestBranchBox = nullptr;
	DialogueNameText = nullptr;
	DialogueTitleText = nullptr;
	SkipLineButton = nullptr;
	DialogueReplyBox = nullptr;
	DialogueFadeAnim = nullptr;
	RepliesFadeAnim = nullptr;
	BranchWidgetClass = NULL;
	ReplyWidgetClass = NULL;
	DialoguePlayerColor = FLinearColor::Yellow;
	DialogueNPCColor = FLinearColor::White;
	DialogueTextSettings.Text = FText::FromString(TEXT("Line"));
	DialogueTextSettings.Alignment = {EExpressiveTextVerticalAlignment::Top, EExpressiveTextHorizontalAlignment::Center};
	DialogueTextSettings.Justification = ETextJustify::Center;
	DialogueTextSettings.UseDefaultFontSize = true;
	DialogueTextSettings.DefaultFontSize = 24;
#if WITH_EDITOR
	if (const FString PathStr = UGCSettings::Get()->BaseTextStyle.ToString(); !PathStr.IsEmpty())
	{
		if (const ConstructorHelpers::FObjectFinder<UExpressiveTextStyleBase> StyleFinder(*PathStr); StyleFinder.Succeeded())
		{
			DialogueTextSettings.DefaultStyle = StyleFinder.Object;		
		}
	}
#endif
	
	NarrativeComponent = nullptr;
	DialogueReplies = {};
	bQuestsHidden = false;
}

void UGCNarrativeWidget::SetQuestsHidden(const bool bInHidden)
{
	if (bQuestsHidden != bInHidden)
	{
		bQuestsHidden = bInHidden;
	}
}

FExpressiveTextSelector UGCNarrativeWidget::ConstructText(const FText& InText)
{
	if (!DialogueTextSettings.DefaultStyle)
	{
		DialogueTextSettings.DefaultStyle = UGCSettings::Get()->BaseTextStyle.LoadSynchronous();
	}
	
	FExpressiveTextSelector RetVal;
	RetVal.InlinedExpressiveText = DialogueTextSettings;
	RetVal.InlinedExpressiveText.Text = InText;
	return RetVal;
}

void UGCNarrativeWidget::InitData(AGCPlayerController* Controller)
{
	Super::InitData(Controller);
	NarrativeComponent = Controller->GetNarrativeComponent();
	
	NarrativeComponent->OnQuestNewState.AddDynamic(this, &UGCNarrativeWidget::OnQuestNewState);
	NarrativeComponent->OnQuestBranchCompleted.AddDynamic(this, &UGCNarrativeWidget::OnQuestBranchCompleted);
	NarrativeComponent->OnQuestTaskCompleted.AddDynamic(this, &UGCNarrativeWidget::OnQuestTaskCompleted);
	NarrativeComponent->OnQuestTaskProgressChanged.AddDynamic(this, &UGCNarrativeWidget::OnQuestTaskProgressChanged);

	NarrativeComponent->OnDialogueFinished.AddDynamic(this, &UGCNarrativeWidget::OnDialogueFinished);
	NarrativeComponent->OnDialogueRepliesAvailable.AddDynamic(this, &UGCNarrativeWidget::OnDialogueRepliesAvailable);
	NarrativeComponent->OnPlayerDialogueLineStarted.AddDynamic(this, &UGCNarrativeWidget::OnDialoguePlayerLineStarted);
	NarrativeComponent->OnPlayerDialogueLineFinished.AddDynamic(this, &UGCNarrativeWidget::OnDialoguePlayerLineFinished);
	NarrativeComponent->OnNPCDialogueLineStarted.AddDynamic(this, &UGCNarrativeWidget::OnDialogueNPCLineStarted);
}

void UGCNarrativeWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SkipLineButton->OnClicked.AddDynamic(this, &UGCNarrativeWidget::OnLineSkipClicked);
}

void UGCNarrativeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	QuestBranchBox->ClearChildren();
	DialogueReplyBox->ClearChildren();
	DialogueNameText->SetText(FText::GetEmpty());
	DialogueTitleText->SetText(FText::GetEmpty());
	SetDialogueLineText(ConstructText(FText::GetEmpty()));
	
	SetQuestsHidden(true);
	PLAY_FIRST_FRAME(DialogueFadeAnim);
	PLAY_FIRST_FRAME(RepliesFadeAnim);
}

void UGCNarrativeWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	const float Current = QuestBox->GetRenderOpacity();
	const float Target = bQuestsHidden ? 0.0f : 1.0f;
	if (Current != Target)
	{
		QuestBox->SetRenderOpacity(FMath::FInterpConstantTo(Current, Target, InDeltaTime, 5.0f));
	}
}

void UGCNarrativeWidget::QuestUpdatedNotify() const
{
	if (UGCMessageWidget* MsgWidget = PlayerController->GetUserWidget<UGCMessageWidget>())
	{
		MsgWidget->QueueNoticeByObject(FGCNoticeData{FText::FromString("Objective Updated"), 1.0f}, this);
	}
}

void UGCNarrativeWidget::RefreshQuestList(const UQuest* Quest, const UQuestBranch* Branch)
{
	QuestBranchBox->ClearChildren();
	if (Branch)
	{
		if (!Branch->DestinationState || Branch->DestinationState->StateNodeType != EStateNodeType::Failure)
		{
			UGCQuestBranchWidget* BranchWidget = CreateWidget<UGCQuestBranchWidget>(GetOwningPlayer(), BranchWidgetClass);
			BranchWidget->InitWidget(Branch);

			QuestBranchBox->AddChild(BranchWidget);
		}

		QuestUpdatedNotify();
		QuestBox->SetRenderOpacity(0.0f);
		SetQuestsHidden(false);
	}
}

void UGCNarrativeWidget::OnQuestNewState(UQuest* Quest, const UQuestState* State)
{
	if (State && !State->Branches.IsEmpty())
	{
		QuestBranchBox->ClearChildren();
		for (const UQuestBranch* Branch : State->Branches)
		{
			if (Branch && !Branch->bHidden)
			{
				UGCQuestBranchWidget* BranchWidget = CreateWidget<UGCQuestBranchWidget>(GetOwningPlayer(), BranchWidgetClass);
				BranchWidget->InitWidget(Branch);

				QuestBranchBox->AddChild(BranchWidget);
			}
		}

		QuestUpdatedNotify();
		QuestBox->SetRenderOpacity(0.0f);
		SetQuestsHidden(false);
	}
	else
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, [this](){
			SetQuestsHidden(true);
		}, 1.0f, false);
	}
}

void UGCNarrativeWidget::OnQuestBranchCompleted(const UQuest* Quest, const UQuestBranch* Branch)
{
	RefreshQuestList(Quest, Branch);
}

void UGCNarrativeWidget::OnQuestTaskCompleted(const UQuest* Quest, const UNarrativeTask* Task, const UQuestBranch* Branch)
{
	if (Branch && Branch->DestinationState && Branch->DestinationState->StateNodeType != EStateNodeType::Failure)
	{
		RefreshQuestList(Quest, Branch);
	}
}

void UGCNarrativeWidget::OnQuestTaskProgressChanged(const UQuest* Quest, const UNarrativeTask* Task, const UQuestBranch* Branch, int32 Current, int32 Target)
{
	if (Branch && Task && Target > 0)
	{
		if ((!Branch->DestinationState || Branch->DestinationState->StateNodeType != EStateNodeType::Failure)
			&& Task->GetProgress() != Task->RequiredQuantity)
		{
			RefreshQuestList(Quest, Branch);
		}
	}
}

void UGCNarrativeWidget::SelectDialogueReply(UDialogueNode_Player* Reply)
{
	NarrativeComponent->SelectDialogueOption(Reply);
	for (const TPair<FName, UGCDialogueOptionWidget*>& DialogueReply : DialogueReplies)
	{
		if (!DialogueReply.Value)
			continue;

		DialogueReply.Value->SetColorAndOpacity(FLinearColor::Gray);
		DialogueReply.Value->DisplayText->SetColorAndOpacity(FLinearColor::White);
		DialogueReply.Value->SetVisibility(ESlateVisibility::HitTestInvisible);
		if (DialogueReply.Key == Reply->GetID())
		{
			DialogueReply.Value->MarkSelected();
		}
	}
	
	PlayAnimation(RepliesFadeAnim);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGCNarrativeWidget::OnLineSkipClicked()
{
	NarrativeComponent->CurrentDialogue->SkipCurrentLine();
}

void UGCNarrativeWidget::OnDialogueFinished(UDialogue* Dialogue)
{
	PlayAnimationReverse(DialogueFadeAnim);

	if (GetAnimationCurrentTime(RepliesFadeAnim) > 0.0f)
	{
		PlayAnimationReverse(RepliesFadeAnim);
	}
}

void UGCNarrativeWidget::OnDialogueRepliesAvailable(UDialogue* Dialogue, const TArray<UDialogueNode_Player*>& PlayerReplies)
{
	TArray<UDialogueNode_Player*> TempReplies = PlayerReplies;
	Algo::Reverse(TempReplies);

	DialogueReplyBox->ClearChildren();
	for (UDialogueNode_Player* Reply : TempReplies)
	{
		UGCDialogueOptionWidget* Widget = CreateWidget<UGCDialogueOptionWidget>(this, ReplyWidgetClass);
		Widget->InitWidget(this, Dialogue, Reply);

		DialogueReplies.Add(Reply->GetID(), Widget);
		DialogueReplyBox->AddChild(Widget);
	}

	PlayAnimation(RepliesFadeAnim);
}

void UGCNarrativeWidget::OnDialoguePlayerLineStarted(UDialogue* Dialogue, UDialogueNode_Player* Node, const FDialogueLine& DialogueLine)
{
	DialogueNameText->SetText(INVTEXT("You"));
	DialogueNameText->SetColorAndOpacity(DialoguePlayerColor);
	DialogueTitleText->SetVisibility(ESlateVisibility::Collapsed);
	SetDialogueLineText(ConstructText(DialogueLine.Text));
	PlayAnimation(DialogueFadeAnim);
}

void UGCNarrativeWidget::OnDialoguePlayerLineFinished(UDialogue* Dialogue, UDialogueNode_Player* Node, const FDialogueLine& DialogueLine)
{
	PlayAnimationReverse(RepliesFadeAnim);
}

void UGCNarrativeWidget::OnDialogueNPCLineStarted(UDialogue* Dialogue, UDialogueNode_NPC* Node, const FDialogueLine& DialogueLine, const FSpeakerInfo& Speaker)
{
	TArray<FString> Args;
	if (Speaker.SpeakerID.ToString().ParseIntoArray(Args, TEXT("|"), true) == 0)
	{
		Args = { Speaker.SpeakerID.ToString() };
	}
	
	DialogueNameText->SetText(FText::FromString(Args[0]));
	DialogueNameText->SetColorAndOpacity(DialogueNPCColor);
	if (Args.IsValidIndex(1))
	{
		DialogueTitleText->SetText(FText::FromString(Args[1]));
		DialogueTitleText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		DialogueTitleText->SetVisibility(ESlateVisibility::Collapsed);
	}

	SetDialogueLineText(ConstructText(DialogueLine.Text));
	PlayAnimation(DialogueFadeAnim);
}
