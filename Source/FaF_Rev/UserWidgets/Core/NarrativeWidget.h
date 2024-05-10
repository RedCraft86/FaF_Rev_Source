// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "NarrativeComponent.h"
#include "NarrativeWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS(Abstract)
class FAF_REV_API UQuestTaskWidgetBase final : public UUserWidget
{
	GENERATED_BODY()

	friend class UQuestBranchWidgetBase;
	
public:

	UQuestTaskWidgetBase(const FObjectInitializer& ObjectInitializer)
		: UUserWidget(ObjectInitializer), DisplayText(nullptr), TaskObject(nullptr)
	{}

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* DisplayText;

protected:

	UPROPERTY(Transient) const UNarrativeTask* TaskObject;

	void InitWidget(const UNarrativeTask* Task);
};

UCLASS(Abstract)
class FAF_REV_API UQuestBranchWidgetBase final : public UUserWidget
{
	GENERATED_BODY()

	friend class UNarrativeWidgetBase;
	
public:

	UQuestBranchWidgetBase(const FObjectInitializer& ObjectInitializer)
		: UUserWidget(ObjectInitializer), DisplayText(nullptr), TaskList(nullptr), BranchObject(nullptr)
	{}

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* DisplayText;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UPanelWidget* TaskList;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuestBranchWidget")
		TSubclassOf<UQuestTaskWidgetBase> TaskWidgetClass;
	
protected:

	UPROPERTY(Transient) const UQuestBranch* BranchObject;

	void InitWidget(const UQuestBranch* Branch);
};

UCLASS(Abstract)
class FAF_REV_API UDialogueOptionWidgetBase final : public UUserWidget
{
	GENERATED_BODY()

	friend class UNarrativeWidgetBase;
	
public:

	UDialogueOptionWidgetBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* DisplayText;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UButton* SelectButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogueOptionWidget", meta = (UIMin = 10, ClampMin = 10))
		uint8 MaxCharCount;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogueOptionWidget")
		FLinearColor SelectedColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogueOptionWidget")
		const UNarrativeDataTask* ReplyTaskType;

protected:

	UPROPERTY(Transient) UNarrativeWidgetBase* ParentUI;
	UPROPERTY(Transient) UDialogueNode_Player* ReplyObject;

	UFUNCTION() void OnOptionClicked();
	
	void MarkSelected() const;
	void InitWidget(UNarrativeWidgetBase* NarrativeWidget, UDialogue* Dialogue, UDialogueNode_Player* Reply);
};

UCLASS(Abstract)
class FAF_REV_API UNarrativeWidgetBase final : public UGTUserWidget
{
	GENERATED_BODY()

	friend class UDialogueOptionWidgetBase;
	
public:

	UNarrativeWidgetBase(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(Transient, meta = (BindWidget))
		UPanelWidget* QuestBranchBox;

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* DialogueNameText;

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* DialogueTitleText;

	UPROPERTY(Transient, meta = (BindWidget))
		class UExprTextBlock* DialogueLineText;

	UPROPERTY(Transient, meta = (BindWidget))
		UButton* SkipLineButton;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UPanelWidget* DialogueReplyBox;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* QuestFadeAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* DialogueFadeAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* RepliesFadeAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* HideFadeAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NarrativeWidget")
		TSubclassOf<UQuestBranchWidgetBase> BranchWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NarrativeWidget")
		TSubclassOf<UDialogueOptionWidgetBase> ReplyWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NarrativeWidget")
		FLinearColor DialoguePlayerColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NarrativeWidget", DisplayName = "Dialogue NPC Color")
		FLinearColor DialogueNPCColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NarrativeWidget")
		FMargin DialogueReplyPadding;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "NarrativeWidget")
		UNarrativeComponent* NarrativeComponent;

	UFUNCTION(BlueprintCallable, Category = "NarrativeWidget")
		void SetQuestsHidden(const bool bInHidden);

	UFUNCTION(BlueprintPure, Category = "NarrativeWidget")
		bool AreQuestsHidden() const { return bQuestsHidden; }
	
protected:

	bool bQuestsHidden;
	float HideCheckTime;
	UPROPERTY(Transient) AWorldSettings* WorldSettings;
	UPROPERTY(Transient) class AFRPlayerBase* PlayerChar;
	UPROPERTY(Transient) TMap<FName, UDialogueOptionWidgetBase*> DialogueReplies;

	void QuestUpdatedNotify();
	void RefreshQuestList(const UQuest* Quest, const UQuestBranch* Branch);
	UFUNCTION() void OnQuestNewState(UQuest* Quest, const UQuestState* NewState);
	UFUNCTION() void OnQuestBranchCompleted(const UQuest* Quest, const UQuestBranch* Branch);
	UFUNCTION() void OnQuestTaskCompleted(const UQuest* Quest, const UNarrativeTask* CompletedTask, const UQuestBranch* Branch);
	UFUNCTION() void OnQuestTaskProgressChanged(const UQuest* Quest, const UNarrativeTask* ProgressedTask, const UQuestBranch* Branch, int32 OldProgress, int32 NewProgress);

	void SelectDialogueReply(UDialogueNode_Player* Reply);
	UFUNCTION() void OnDialogueFinished(UDialogue* Dialogue, const bool BStartingNewDialogue);
	UFUNCTION() void OnDialogueRepliesAvailable(UDialogue* Dialogue, const TArray<UDialogueNode_Player*>& PlayerReplies);
	UFUNCTION() void OnDialoguePlayerLineStarted(UDialogue* Dialogue, UDialogueNode_Player* Node, const FDialogueLine& DialogueLine);
	UFUNCTION() void OnDialoguePlayerLineFinished(UDialogue* Dialogue, UDialogueNode_Player* Node, const FDialogueLine& DialogueLine);
	UFUNCTION() void OnDialogueNPCLineStarted(UDialogue* Dialogue, UDialogueNode_NPC* Node, const FDialogueLine& DialogueLine, const FSpeakerInfo& Speaker);
	UFUNCTION() void OnLineSkipClicked();
	
	void HideCheck();
	virtual void InitWidget() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
