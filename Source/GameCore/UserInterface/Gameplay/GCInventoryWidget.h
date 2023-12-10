// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "UserInterface/GCUserWidget.h"
#include "Inventory/GCInventoryTypes.h"
#include "GCInventoryWidget.generated.h"

class UImage;
class UButton;
class USizeBox;
class UTextBlock;
class URichTextBlock;

UCLASS(Abstract, DisplayName = "Inventory Slot Widget Base")
class GAMECORE_API UGCInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

	friend class UGCInventoryWidget;
	
public:

	UGCInventorySlotWidget(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(Transient, meta = (BindWidget))
		UImage* BackgroundImage;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UImage* IconImage;

	UPROPERTY(Transient, meta = (BindWidget))
		UImage* IconEquip;

	UPROPERTY(Transient, meta = (BindWidget))
		UImage* IconData;

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* AmountText;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UButton* SlotButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InventorySlotWidget")
		FLinearColor NormalOutlineColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InventorySlotWidget")
		FLinearColor SelectedOutlineColor;
	
	void InitData();

protected:
	
	UPROPERTY(BlueprintReadOnly, Category = "InventorySlotWidget")
		FName ItemID;
	
	UPROPERTY(BlueprintReadOnly, Category = "InventorySlotWidget")
		FGCInvItemSlotData SlotData;

	UPROPERTY(BlueprintReadOnly, Category = "InventorySlotWidget")
		bool bEquipped;

	UGCInventoryWidget* ParentUI;

	void MarkSelected(const bool bSelected) const;

	UFUNCTION()
		void OnClicked();
};

UCLASS(Abstract, DisplayName = "Inventory Widget Base")
class GAMECORE_API UGCInventoryWidget : public UGCUserWidget
{
	GENERATED_BODY()

	friend class UGCInventorySlotWidget;
	
public:

	UGCInventoryWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		UPanelWidget* SlotsBox;

	UPROPERTY(Transient, meta = (BindWidget))
		UImage* ItemPreviewImage;

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* ItemTitleText;

	UPROPERTY(Transient, meta = (BindWidgetOptional))
		URichTextBlock* ItemDescText;

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* ItemTypeText;

	UPROPERTY(Transient, meta = (BindWidget))
		UPanelWidget* EquipStateBox;

	UPROPERTY(Transient, meta = (BindWidget))
		UButton* UsageButton;

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* UsageText;

	UPROPERTY(Transient, meta = (BindWidget))
		UTextBlock* ViewingTitle;

	UPROPERTY(Transient, meta = (BindWidget))
		UPanelWidget* ViewContentBox;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UImage* ViewContentImage;

	UPROPERTY(Transient, meta = (BindWidget))
		USizeBox* ReadContentBox;
	
	UPROPERTY(Transient, meta = (BindWidgetOptional))
		URichTextBlock* ReadContentText;

	UPROPERTY(Transient, meta = (BindWidget))
		UButton* FinishViewingButton;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* SlotsFadeAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* DescFadeAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* ViewingFadeAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InventoryWidget")
		TSubclassOf<UGCInventorySlotWidget> SlotWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InventoryWidget")
		float ImageDescHeight;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InventoryWidget")
		FText EquipItemLabel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InventoryWidget")
		FText UnequipItemLabel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InventoryWidget")
		FText ViewItemLabel;

	UFUNCTION(BlueprintImplementableEvent, meta = (ForceAsFunction = true))
		void SetItemDescText(const FText& InText);

	UFUNCTION(BlueprintImplementableEvent, meta = (ForceAsFunction = true))
		void SetReadContentText(const FText& InText);
	
protected:

	UPROPERTY(Transient, BlueprintReadOnly)
		class UGCInventoryManager* InventoryManager;

	UPROPERTY(Transient, BlueprintReadOnly)
		class AGCInspectionActor* InspectionActor;

	UPROPERTY(Transient, BlueprintReadOnly)
		 TMap<FName, UGCInventorySlotWidget*> SlotWidgets;

	FName SelectedItem;
	
	void OnInventoryUpdate();
	
	void UpdateItemInfo();
	void SelectSlot(const FName& ItemID);
	virtual void InitData(AGCPlayerController* Controller) override;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
		void OnUsageClicked();

	UFUNCTION()
		void OnReadFinishClicked();
};
