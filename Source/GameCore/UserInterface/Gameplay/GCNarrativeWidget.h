// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "Quest.h"
#include "Dialogue.h"
#include "Handles/ExpressiveTextSelector.h"
#include "UserInterface/GCUserWidget.h"
#include "GCNarrativeWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS(Abstract, DisplayName = "Quest Task Widget Base")
class GAMECORE_API UGCQuestTaskWidget : public UUserWidget
{
	GENERATED_BODY()

	friend class UGCQuestBranchWidget;
	
public:

	UGCQuestTaskWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* DisplayText;

protected:

	UPROPERTY(BlueprintReadOnly, Category = "QuestTaskWidget")
		const UNarrativeTask* TaskObject;

	void InitWidget(const UNarrativeTask* Task);
};

UCLASS(Abstract, DisplayName = "Quest Branch Widget Base")
class GAMECORE_API UGCQuestBranchWidget : public UUserWidget
{
	GENERATED_BODY()

	friend class UGCNarrativeWidget;
	
public:

	UGCQuestBranchWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* DisplayText;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UPanelWidget* NormalTaskBox;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UPanelWidget* OptionalTaskBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuestBranchWidget")
		TSubclassOf<UGCQuestTaskWidget> TaskWidgetClass;
	
protected:

	UPROPERTY(BlueprintReadOnly, Category = "QuestTaskWidget")
		const UQuestBranch* BranchObject;

	void InitWidget(const UQuestBranch* Branch);
};

UCLASS(Abstract, DisplayName = "Dialogue Option Widget Base")
class GAMECORE_API UGCDialogueOptionWidget : public UUserWidget
{
	GENERATED_BODY()

	friend class UGCNarrativeWidget;
	
public:

	UGCDialogueOptionWidget(const FObjectInitializer& ObjectInitializer);

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

	UPROPERTY(Transient)
		UGCNarrativeWidget* ParentUI;

	UPROPERTY(Transient)
		UDialogueNode_Player* ReplyObject;

	virtual void NativeOnInitialized() override;
	void InitWidget(UGCNarrativeWidget* NarrativeWidget, UDialogue* Dialogue, UDialogueNode_Player* Reply);
	void MarkSelected() const;

	UFUNCTION()
		void OnOptionClicked();
};

UCLASS(Abstract, DisplayName = "Narrative Widget Base")
class GAMECORE_API UGCNarrativeWidget : public UGCUserWidget
{
	GENERATED_BODY()

	friend class UGCDialogueOptionWidget;
	
public:

	UGCNarrativeWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		UPanelWidget* QuestBox;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UPanelWidget* QuestBranchBox;

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* DialogueNameText;

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* DialogueTitleText;

	UPROPERTY(Transient, meta = (BindWidget))
		UButton* SkipLineButton;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UPanelWidget* DialogueReplyBox;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* DialogueFadeAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* RepliesFadeAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NarrativeWidget")
		TSubclassOf<UGCQuestBranchWidget> BranchWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NarrativeWidget")
		TSubclassOf<UGCDialogueOptionWidget> ReplyWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NarrativeWidget")
		FLinearColor DialoguePlayerColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NarrativeWidget", DisplayName = "Dialogue NPC Color")
		FLinearColor DialogueNPCColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NarrativeWidget")
		FExpressiveTextFields DialogueTextSettings;

	UFUNCTION(BlueprintImplementableEvent, meta = (ForceAsFunction = true))
		void SetDialogueLineText(const FExpressiveTextSelector& InText);
	
	UFUNCTION(BlueprintCallable, Category = "NarrativeWidget")
		void SetQuestsHidden(const bool bInHidden);

	UFUNCTION(BlueprintPure, Category = "NarrativeWidget")
		bool AreQuestsHidden() const { return bQuestsHidden; }

	class UGCNarrativeComponent* GetNarrative() const { return NarrativeComponent; }
	
protected:

	UPROPERTY(Transient, BlueprintReadOnly)
		UGCNarrativeComponent* NarrativeComponent;

	UPROPERTY(Transient)
		TMap<FName, UGCDialogueOptionWidget*> DialogueReplies;

	bool bQuestsHidden;

	FExpressiveTextSelector ConstructText(const FText& InText);
	virtual void InitData(AGCPlayerController* Controller) override;
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	// Quests
	void QuestUpdatedNotify() const;
	void RefreshQuestList(const UQuest* Quest, const UQuestBranch* Branch);
	
	UFUNCTION() void OnQuestNewState(UQuest* Quest, const UQuestState* State);
	UFUNCTION() void OnQuestBranchCompleted(const UQuest* Quest, const UQuestBranch* Branch);
	UFUNCTION() void OnQuestTaskCompleted(const UQuest* Quest, const UNarrativeTask* Task, const UQuestBranch* Branch);
	UFUNCTION() void OnQuestTaskProgressChanged(const UQuest* Quest, const UNarrativeTask* Task, const UQuestBranch* Branch, int32 Current, int32 Target);

	// Dialogues
	void SelectDialogueReply(UDialogueNode_Player* Reply);
	
	UFUNCTION() void OnLineSkipClicked();
	UFUNCTION() void OnDialogueFinished(UDialogue* Dialogue);
	UFUNCTION() void OnDialogueRepliesAvailable(UDialogue* Dialogue, const TArray<UDialogueNode_Player*>& PlayerReplies);
	UFUNCTION() void OnDialoguePlayerLineStarted(UDialogue* Dialogue, UDialogueNode_Player* Node, const FDialogueLine& DialogueLine);
	UFUNCTION() void OnDialoguePlayerLineFinished(UDialogue* Dialogue, UDialogueNode_Player* Node, const FDialogueLine& DialogueLine);
	UFUNCTION() void OnDialogueNPCLineStarted(UDialogue* Dialogue, UDialogueNode_NPC* Node, const FDialogueLine& DialogueLine, const FSpeakerInfo& Speaker);
};
