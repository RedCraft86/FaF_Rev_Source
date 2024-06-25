// Copyright (C) RedCraft86. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "InventoryWidget.h"
#include "ExprTextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryItemData.h"
#include "FRGameMode.h"

void UInventorySlotWidgetBase::InitData(const FGuid& InSlotKey, const bool bEquipped)
{
	SlotKey = InSlotKey;
	const FInventorySlotData& SlotData = ParentUI->Inventory->ItemSlots[InSlotKey];
	const UInventoryItemData* ItemData = SlotData.GetItemData<UInventoryItemData>();
	
	IconImage->SetBrushFromTexture(ItemData->DisplayIcon.LoadSynchronous(), false);
	IconEquip->SetVisibility(bEquipped ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	AmountText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), SlotData.Amount, ItemData->GetStackLimit())));

	SlotButton->SetToolTipText(ItemData->GetDisplayName(SlotData.Metadata));
	SlotButton->OnClicked.AddDynamic(this, &UInventorySlotWidgetBase::OnClicked);
}

void UInventorySlotWidgetBase::OnClicked()
{
	ParentUI->SelectSlot(SlotKey);
}

void UInventorySlotWidgetBase::MarkSelected(const bool bSelected) const
{
	BackgroundImage->SetColorAndOpacity(bSelected ? SelectedOutlineColor : NormalOutlineColor);
}

UInventoryWidgetBase::UInventoryWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), SlotsBox(nullptr), ItemPreviewImage(nullptr), ItemTitleText(nullptr)
	, ItemDescText(nullptr), ItemTypeText(nullptr), MultiUseKeyText(nullptr), EquipStateBox(nullptr)
	, UsageButton(nullptr), UsageText(nullptr), ViewingTitle(nullptr), ViewContentBox(nullptr), ViewContentImage(nullptr)
	, ReadContentBox(nullptr), ReadContentText(nullptr), FinishViewingButton(nullptr), SlotsFadeAnim(nullptr)
	, DescFadeAnim(nullptr), ViewingFadeAnim(nullptr), SlotWidgetClass(nullptr), ImageDescHeight(200.0f)
{
	ZOrder = 94;
	bAutoAdd = false;
}

void UInventoryWidgetBase::SelectSlot(const FGuid& InKey)
{
	if (SelectedKey == InKey)
	{
		if (const TObjectPtr<UInventorySlotWidgetBase> SlotObj = SlotWidgets.FindRef(SelectedKey))
		{
			SlotObj->MarkSelected(true);
		}

		return;
	}
	if (!Inventory || !Inventory->IsInInventory()) return;
	
	SelectedKey = {};
	if (SlotWidgets.Contains(InKey))
	{
		SelectedKey = InKey;
		for (const TPair<FGuid, TObjectPtr<UInventorySlotWidgetBase>>& SlotWidget : SlotWidgets)
		{
			if (!SlotWidget.Value) continue;
			SlotWidget.Value->MarkSelected(SlotWidget.Key == SelectedKey);
		}
	}
	
	RefreshInfo();
}

void UInventoryWidgetBase::RefreshSlots()
{
	if (!Inventory || !Inventory->IsInInventory()) return;
	EquipmentKey = Inventory->GetEquipmentData().ItemID;
	SlotKeys = Inventory->GetSortedSlots((EInventoryItemType)SlotFilter->GetSelectedIndex());

	SlotsBox->ClearChildren();
	SlotWidgets.Empty(SlotKeys.Num());
	if (SlotWidgets.IsEmpty() && SlotKeys.IsEmpty())
	{
		SelectedKey = {};
		RefreshInfo();
		return;
	}

	for (const FGuid& Key : SlotKeys)
	{
		UInventorySlotWidgetBase* NewSlot = CreateWidget<UInventorySlotWidgetBase>(this, SlotWidgetClass);
		NewSlot->ParentUI = this;
		NewSlot->InitData(Key, Key == EquipmentKey);
		SlotWidgets.Add(Key, NewSlot);
		SlotsBox->AddChild(NewSlot);
	}
	
	if (SlotKeys.IsEmpty())
	{
		SelectSlot({});
	}
	else if (!SlotKeys.Contains(SelectedKey))
	{
		SelectSlot(SlotKeys[0]);
	}
	else if (const TObjectPtr<UInventorySlotWidgetBase> SlotObj = SlotWidgets.FindRef(SelectedKey))
	{
		RefreshInfo();
		SlotObj->MarkSelected(true);
	}

	PlayAnimation(SlotsFadeAnim);
}

void UInventoryWidgetBase::RefreshInfo()
{
	if (!Inventory || !Inventory->IsInInventory()) return;
	
	UsageText->SetText(FText::GetEmpty());
	UsageButton->SetVisibility(ESlateVisibility::Collapsed);
	EquipStateBox->SetVisibility(ESlateVisibility::Collapsed);
	MultiUseKeyText->SetVisibility(ESlateVisibility::Collapsed);
	if (SelectedKey.IsValid() && SlotWidgets.Contains(SelectedKey) && Inventory->IsValidSlot(SelectedKey))
	{
		const FInventorySlotData SlotData = Inventory->GetInventoryRef()[SelectedKey];
		const UInventoryItemData* ItemData = SlotData.GetItemData<UInventoryItemData>();
		const bool bEquipped = Inventory->GetEquipmentData().ItemID == SelectedKey;

		ItemDescText->SetText(ItemData->GetDescription(SlotData.Metadata));
		ItemTitleText->SetText(ItemData->GetDisplayName(SlotData.Metadata));
		ItemTypeText->SetText(FText::Format(INVTEXT("Type: {0}"), FText::FromString(LexToString(ItemData->ItemType))));
		if (SlotData.Metadata.FindRef(NativeItemKeys::SingleKey).IsEmpty())
			MultiUseKeyText->SetVisibility(ESlateVisibility::HitTestInvisible);

		if (bEquipped) EquipStateBox->SetVisibility(ESlateVisibility::HitTestInvisible);
		
		switch (ItemData->ItemType)
		{
		case EInventoryItemType::Viewable:
			if (ItemData->ViewImage || !ItemData->ViewText.IsEmptyOrWhitespace())
			{
				UsageButton->SetVisibility(ESlateVisibility::Visible);
				UsageText->SetText(INVTEXT("Read/View"));
			}
			break;
		case EInventoryItemType::Consumable:
			if (ItemData->ConsumableClass)
			{
				UsageButton->SetVisibility(ESlateVisibility::Visible);
				UsageText->SetText(ItemData->ConsumeDisplayText);
			}
			break;
		case EInventoryItemType::Equipment:
			if (ItemData->EquipmentClass)
			{
				UsageButton->SetVisibility(ESlateVisibility::Visible);
				UsageText->SetText(bEquipped ? INVTEXT("Unequip") : INVTEXT("Equip"));
			}
			break;
		default: break;
		}

		if (Inventory->GetInventoryPreview() && Inventory->GetInventoryPreview()->SetItem(SelectedKey))
		{
			PreviewControlBox->SetVisibility(ESlateVisibility::HitTestInvisible);
			ItemPreviewImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			PreviewControlBox->SetVisibility(ESlateVisibility::Collapsed);
			ItemPreviewImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		PreviewControlBox->SetVisibility(ESlateVisibility::Collapsed);
		ItemPreviewImage->SetVisibility(ESlateVisibility::Collapsed);
		ItemDescText->SetText(INVTEXT("No item selected"));
		ItemTypeText->SetText(INVTEXT("Type: None"));
		ItemTitleText->SetText(INVTEXT("None"));
	}

	PlayAnimation(DescFadeAnim);
}

void UInventoryWidgetBase::OnUsageClicked()
{
	if (!Inventory || !Inventory->IsInInventory() || !SelectedKey.IsValid()
		|| !Inventory->IsValidSlot(SelectedKey)) return;

	const FInventorySlotData SlotData = Inventory->GetInventoryRef()[SelectedKey];
	const UInventoryItemData* ItemData = SlotData.GetItemData<UInventoryItemData>();
	switch (ItemData->ItemType)
	{
	case EInventoryItemType::Consumable:
		Inventory->ConsumeItem(SelectedKey);
		break;

	case EInventoryItemType::Equipment:
		if (Inventory->GetEquipmentData().ItemID == SelectedKey) Inventory->UnequipItem();
		else Inventory->EquipItem(SelectedKey);
		break;

	case EInventoryItemType::Viewable:
		ViewingTitle->SetText(ItemTitleText->GetText());
		ReadContentText->SetText(ItemData->ViewText);
		ReadContentBox->SetVisibility(ItemData->ViewText.IsEmptyOrWhitespace() ?
			ESlateVisibility::Collapsed : ESlateVisibility::Visible);
		
		ViewContentImage->SetBrushFromTexture(ItemData->ViewImage);
		ViewContentBox->SetVisibility(IsValid(ViewContentImage->GetBrush().GetResourceObject()) ?
			ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

		if (ItemData->ViewImage && !ItemData->ViewText.IsEmptyOrWhitespace())
		{
			ReadContentBox->SetHeightOverride(ImageDescHeight);
		}
		else ReadContentBox->ClearHeightOverride();
		PlayAnimation(ViewingFadeAnim);
		break;

	default: break;
	}
}

void UInventoryWidgetBase::OnReadFinishClicked()
{
	PlayAnimationReverse(ViewingFadeAnim);
}

void UInventoryWidgetBase::OnTypeSelected(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	RefreshSlots();
}

void UInventoryWidgetBase::InitWidget()
{
	Super::InitWidget();
	UsageButton->OnClicked.AddDynamic(this, &UInventoryWidgetBase::OnUsageClicked);
	FinishViewingButton->OnClicked.AddDynamic(this, &UInventoryWidgetBase::OnReadFinishClicked);

	SlotFilter->ClearOptions();
	for (const EInventoryItemType Type : TEnumRange<EInventoryItemType>())
	{
		SlotFilter->AddOption(LexToString(Type));
	}
	SlotFilter->SetSelectedIndex(0);
	SlotFilter->OnSelectionChanged.AddDynamic(this, &UInventoryWidgetBase::OnTypeSelected);

	Inventory = GetGameMode<AFRGameModeBase>()->Inventory;
	Inventory->OnUpdate.AddUObject(this, &UInventoryWidgetBase::RefreshSlots);
}

void UInventoryWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		if (!Inventory) Inventory = GetGameMode<AFRGameModeBase>()->Inventory;
		RefreshSlots();
		RefreshInfo();	
	});
}

void UInventoryWidgetBase::NativeDestruct()
{
	Super::NativeDestruct();
	SetAnimationCurrentTime(ViewingFadeAnim, 0.0f);
}
