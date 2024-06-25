// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "InputModeData.h"
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
		TObjectPtr<UTextBlock> DisplayText;

protected:

	UPROPERTY(Transient) TObjectPtr<const UNarrativeTask> TaskObject;

	void InitWidget(const UNarrativeTask* Task);
};

UCLASS(Abstract)
class FAF_REV_API UQuestBranchWidgetBase final : public UUserWidget
{
	GENERATED_BODY()

	friend class UNarrativeWidgetBase;
	
public:

	UQuestBranchWidgetBase(const FObjectInitializer& ObjectInitializer)
		: UUserWidget(ObjectInitializer), DisplayText(nullptr), TaskList(nullptr), bUseTitle(false), BranchObject(nullptr)
	{}

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> DisplayText;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UPanelWidget> TaskList;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuestBranchWidget")
		TSubclassOf<UQuestTaskWidgetBase> TaskWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuestBranchWidget")
		bool bUseTitle;
	
protected:

	UPROPERTY(Transient) TObjectPtr<const UQuestBranch> BranchObject;

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
		TObjectPtr<UTextBlock> DisplayText;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> SelectButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogueOptionWidget", meta = (UIMin = 10, ClampMin = 10))
		uint8 MaxCharCount;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogueOptionWidget")
		FLinearColor SelectedColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogueOptionWidget")
		TObjectPtr<const UNarrativeDataTask> ReplyTaskType;

protected:

	UPROPERTY(Transient) TObjectPtr<UNarrativeWidgetBase> ParentUI;
	UPROPERTY(Transient) TObjectPtr<UDialogueNode_Player> ReplyObject;

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
		TObjectPtr<UPanelWidget> QuestBranchBox;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> DialogueNameText;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> DialogueTitleText;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UExprTextBlock> DialogueLineText;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> SkipLineButton;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UPanelWidget> DialogueReplyBox;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> QuestFadeAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> DialogueFadeAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> RepliesFadeAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> HideFadeAnim;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NarrativeWidget")
		bool bNotifyObjectiveUpdates;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "NarrativeWidget")
		TObjectPtr<UNarrativeComponent> NarrativeComponent;

	UFUNCTION(BlueprintCallable, Category = "NarrativeWidget")
		void SetQuestsHidden(const bool bInHidden);

	UFUNCTION(BlueprintPure, Category = "NarrativeWidget")
		bool AreQuestsHidden() const { return bHideQuests; }
	
protected:

	bool bHideQuests;
	bool bAutoHidden;
	float HideCheckTime;
	FGameInputModeData CachedInputMode;
	UPROPERTY(Transient) TObjectPtr<class AFRPlayerBase> PlayerChar;
	UPROPERTY(Transient) TObjectPtr<const AWorldSettings> WorldSettings;
	UPROPERTY(Transient) TMap<FName, TObjectPtr<UDialogueOptionWidgetBase>> DialogueReplies;

	void QuestUpdatedNotify();
	void RefreshQuestList(const UQuest* Quest, const UQuestBranch* Branch);
	UFUNCTION() void OnQuestNewState(UQuest* Quest, const UQuestState* NewState);
	UFUNCTION() void OnQuestTaskCompleted(const UQuest* Quest, const UNarrativeTask* CompletedTask, const UQuestBranch* Branch);
	UFUNCTION() void OnQuestTaskProgressChanged(const UQuest* Quest, const UNarrativeTask* ProgressedTask, const UQuestBranch* Branch, int32 OldProgress, int32 NewProgress);

	void SelectDialogueReply(UDialogueNode_Player* Reply);
	UFUNCTION() void OnDialogueBegan(UDialogue* Dialogue);
	UFUNCTION() void OnDialogueFinished(UDialogue* Dialogue, const bool bStartingNewDialogue);
	UFUNCTION() void OnDialogueRepliesAvailable(UDialogue* Dialogue, const TArray<UDialogueNode_Player*>& PlayerReplies);
	UFUNCTION() void OnDialoguePlayerLineStarted(UDialogue* Dialogue, UDialogueNode_Player* Node, const FDialogueLine& DialogueLine);
	UFUNCTION() void OnDialoguePlayerLineFinished(UDialogue* Dialogue, UDialogueNode_Player* Node, const FDialogueLine& DialogueLine);
	UFUNCTION() void OnDialogueNPCLineStarted(UDialogue* Dialogue, UDialogueNode_NPC* Node, const FDialogueLine& DialogueLine, const FSpeakerInfo& Speaker);
	UFUNCTION() void OnLineSkipClicked();
	
	void HideCheck();
	virtual void InitWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
