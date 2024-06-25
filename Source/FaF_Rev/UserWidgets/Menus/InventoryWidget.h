// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "InventoryWidget.generated.h"

class USizeBox;
class UExprTextBlock;
class UTextBlock;
class UButton;
class UImage;

UCLASS(Abstract)
class FAF_REV_API UInventorySlotWidgetBase final : public UUserWidget
{
	GENERATED_BODY()

	friend class UInventoryWidgetBase;
	
public:

	UInventorySlotWidgetBase(const FObjectInitializer& ObjectInitializer)
		: UUserWidget(ObjectInitializer), BackgroundImage(nullptr), IconImage(nullptr)
		, IconEquip(nullptr), AmountText(nullptr), SlotButton(nullptr)
		, NormalOutlineColor(0.025f, 0.025f, 0.025f, 1.0f)
		, SelectedOutlineColor(0.75f, 0.75f, 0.0f, 1.0f)
		, SlotKey({}), ParentUI(nullptr)
	{}
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UImage> BackgroundImage;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UImage> IconImage;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UImage> IconEquip;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> AmountText;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> SlotButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InventorySlotWidget")
		FLinearColor NormalOutlineColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InventorySlotWidget")
		FLinearColor SelectedOutlineColor;
	
	void InitData(const FGuid& InSlotKey, const bool bEquipped);

protected:
	
	FGuid SlotKey;
	UPROPERTY(Transient) TObjectPtr<UInventoryWidgetBase> ParentUI;
	
	UFUNCTION() void OnClicked();
	void MarkSelected(const bool bSelected) const;
};

UCLASS(Abstract)
class FAF_REV_API UInventoryWidgetBase final : public UGTUserWidget
{
	GENERATED_BODY()

	friend class UInventoryComponent;
	friend class UInventorySlotWidgetBase;

public:

	UInventoryWidgetBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UComboBoxString> SlotFilter;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UPanelWidget> SlotsBox;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UImage> ItemPreviewImage;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UPanelWidget> PreviewControlBox;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> ItemTitleText;

	UPROPERTY(Transient, meta = (BindWidgetOptional))
		TObjectPtr<UExprTextBlock> ItemDescText;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> ItemTypeText;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> MultiUseKeyText;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UPanelWidget> EquipStateBox;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> UsageButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> UsageText;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> ViewingTitle;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UPanelWidget> ViewContentBox;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UImage> ViewContentImage;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<USizeBox> ReadContentBox;
	
	UPROPERTY(Transient, meta = (BindWidgetOptional))
		TObjectPtr<UExprTextBlock> ReadContentText;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> FinishViewingButton;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> SlotsFadeAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> DescFadeAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ViewingFadeAnim;

	UPROPERTY(EditAnywhere, Category = "InventoryWidget")
		TSubclassOf<UInventorySlotWidgetBase> SlotWidgetClass;

	UPROPERTY(EditAnywhere, Category = "InventoryWidget")
		float ImageDescHeight;
	
	void SelectSlot(const FGuid& InKey);
	
protected:

	FGuid SelectedKey;
	FGuid EquipmentKey;
	TArray<FGuid> SlotKeys;
	UPROPERTY(Transient) TMap<FGuid, TObjectPtr<UInventorySlotWidgetBase>> SlotWidgets;
	UPROPERTY(Transient) TObjectPtr<UInventoryComponent> Inventory;
	
	void RefreshSlots();
	void RefreshInfo();

	UFUNCTION() void OnUsageClicked();
	UFUNCTION() void OnReadFinishClicked();
	UFUNCTION() void OnTypeSelected(FString SelectedItem, ESelectInfo::Type SelectionType);
	
	virtual void InitWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
