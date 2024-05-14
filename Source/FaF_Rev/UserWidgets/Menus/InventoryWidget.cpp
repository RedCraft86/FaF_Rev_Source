// Copyright (C) RedCraft86. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "InventoryWidget.h"
#include "ExprTextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryItemData.h"
#include "FRGameMode.h"

#define GET_INVENTORY GetGameMode<AFRGameModeBase>()->Inventory
#define LOCTEXT_NAMESPACE "Inventory"

void UInventorySlotWidgetBase::InitData(const FGuid& InSlotKey, const bool bEquipped)
{
	const FInventorySlotData& SlotData = ParentUI->GET_INVENTORY->ItemSlots[InSlotKey];
	const UInventoryItemData* ItemData = SlotData.GetItemData<UInventoryItemData>();
	
	IconImage->SetBrushFromTexture(ItemData->DisplayIcon.LoadSynchronous(), false);
	IconEquip->SetVisibility(bEquipped ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	AmountText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), SlotData.Amount, ItemData->GetStackLimit())));

	SlotButton->SetToolTipText(ItemData->DisplayName);
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
	, ItemDescText(nullptr), ItemTypeText(nullptr), EquipStateBox(nullptr), UsageButton(nullptr)
	, UsageText(nullptr), ViewingTitle(nullptr), ViewContentBox(nullptr), ViewContentImage(nullptr)
	, ReadContentBox(nullptr), ReadContentText(nullptr), FinishViewingButton(nullptr), SlotsFadeAnim(nullptr)
	, DescFadeAnim(nullptr), ViewingFadeAnim(nullptr), SlotWidgetClass(nullptr), ImageDescHeight(200.0f)
	, EquipItemLabel(LOCTEXT("EquipLabel", "Equip")), UnequipItemLabel(LOCTEXT("UnequipLabel", "Unequip"))
	, ViewItemLabel(LOCTEXT("ViewLabel", "Read/View"))
{
	ZOrder = 97;
	bAutoAdd = false;
}

void UInventoryWidgetBase::SelectSlot(const FGuid& InKey)
{
	if (SelectedKey == InKey) return;

	SelectedKey = {};
	if (SlotWidgets.Contains(InKey))
	{
		SelectedKey = InKey;
		for (const TPair<FGuid, TObjectPtr<UInventorySlotWidgetBase>>& SlotWidget : SlotWidgets)
		{
			if (!SlotWidget.Value) continue;
			if (SlotWidget.Key == SelectedKey)
			{
				SlotWidget.Value->MarkSelected(true);
			}
			else
			{
				SlotWidget.Value->MarkSelected(false);
			}
		}
	}

	UpdateItemInfo();
}

void UInventoryWidgetBase::RefreshUI()
{
	if (IsInViewport())
	{
		UInventoryComponent* Inventory = GET_INVENTORY;
		EquipmentKey = Inventory->GetEquipmentData().ItemID;
		SlotKeys = Inventory->GetSortedSlots();
		if (SlotWidgets.IsEmpty() && SlotKeys.IsEmpty())
		{
			return;
		}
		
		SlotWidgets.Empty();
		SlotsBox->ClearChildren();
		for (const FGuid& Key : SlotKeys)
		{
			UInventorySlotWidgetBase* Slot = CreateWidget<UInventorySlotWidgetBase>(this, SlotWidgetClass);
			Slot->ParentUI = this;
			Slot->InitData(Key, Key == EquipmentKey);
			SlotWidgets.Add(Key, Slot);
			SlotsBox->AddChild(Slot);
		}

		GetWorld()->GetTimerManager().SetTimerForNextTick([this](){
			if (SlotKeys.IsEmpty())
			{
				SelectSlot({});
			}
			else if (!SlotKeys.Contains(SelectedKey))
			{
				SelectSlot(SlotKeys[0]);
			}
			else if (TObjectPtr<UInventorySlotWidgetBase> const* SlotObj = SlotWidgets.Find(SelectedKey); SlotObj && *SlotObj)
			{
				UpdateItemInfo();
				(*SlotObj)->MarkSelected(true);
			}
		});
		
		PlayAnimation(SlotsFadeAnim);
	}
}

void UInventoryWidgetBase::UpdateItemInfo()
{
	ItemPreviewImage->SetVisibility(ESlateVisibility::Collapsed);
	ItemTitleText->SetText(FText::FromString(TEXT("None Selected")));
	if (ItemDescText)
	{
		ItemDescText->SetText(FText::GetEmpty());
	}

	ItemTypeText->SetText(FText::GetEmpty());
	EquipStateBox->SetVisibility(ESlateVisibility::Collapsed);
	UsageButton->SetVisibility(ESlateVisibility::Collapsed);
	UsageText->SetText(FText::GetEmpty());
	
	if (SelectedKey.IsValid() && SlotWidgets.Contains(SelectedKey))
	{
		const UInventoryComponent* Inventory = GET_INVENTORY;
		const FInventorySlotData SlotData = Inventory->GetInventory()[SelectedKey];
		const UInventoryItemData* ItemData = SlotData.GetItemData<UInventoryItemData>();
		const bool bEquipped = Inventory->GetEquipmentData().ItemID == SelectedKey;
		
		ItemTitleText->SetText(ItemData->DisplayName);
		ItemDescText->SetText(ItemData->Description);
		ItemTypeText->SetText(FText::FromString(TEXT("Type: ") + ItemData->GetTypeString()));
		if (bEquipped)
		{
			EquipStateBox->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		
		switch (ItemData->ItemType)
		{
		case EInventoryItemType::Viewable:
			//if (Data.ViewImage || !Data.ViewText.IsEmptyOrWhitespace())
			{
				UsageButton->SetVisibility(ESlateVisibility::Visible);
				UsageText->SetText(ViewItemLabel);
			}
			break;
			
		case EInventoryItemType::Consumable:
			//if (Data.ConsumableClass != NULL)
			{
				UsageButton->SetVisibility(ESlateVisibility::Visible);
				//UsageText->SetText(ItemData->UsageLabel);
			}
			break;
			
		case EInventoryItemType::Equipment:
			//if (Data.EquipmentClass != NULL)
			{
				UsageButton->SetVisibility(ESlateVisibility::Visible);
				UsageText->SetText(bEquipped ? UnequipItemLabel : EquipItemLabel);
			}
			break;

		default: break;
		}

		if (Inventory->GetInspectionActor())
		{
			ItemPreviewImage->SetVisibility(ESlateVisibility::HitTestInvisible);
			Inventory->GetInspectionActor()->SetItem(SelectedKey);
		}
	}
	
	PlayAnimation(DescFadeAnim);
}

void UInventoryWidgetBase::OnUsageClicked()
{
}

void UInventoryWidgetBase::OnReadFinishClicked()
{
	PlayAnimationReverse(ViewingFadeAnim);
}

void UInventoryWidgetBase::InitWidget()
{
	Super::InitWidget();
	UsageButton->OnClicked.AddDynamic(this, &UInventoryWidgetBase::OnUsageClicked);
	FinishViewingButton->OnClicked.AddDynamic(this, &UInventoryWidgetBase::OnReadFinishClicked);
	GET_INVENTORY->OnUpdate.AddUObject(this, &UInventoryWidgetBase::RefreshUI);
}

void UInventoryWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshUI();
	GetGameMode<AFRGameModeBase>()->SetGameInputMode(EGameInputMode::GameAndUI, true,
		EMouseLockMode::LockAlways, false, this);

	SlotsBox->ClearChildren();
	UpdateItemInfo();

	if (const AInspectionActor* Actor = GET_INVENTORY->GetInspectionActor())
	{
		FSlateBrush Brush = ItemPreviewImage->GetBrush();
		Brush.SetResourceObject(Actor->InspectionCapture->TextureTarget);
		ItemPreviewImage->SetBrush(Brush);
	}
}

void UInventoryWidgetBase::NativeDestruct()
{
	Super::NativeDestruct();
	SetAnimationCurrentTime(ViewingFadeAnim, 0.0f);
	GetGameMode<AFRGameModeBase>()->SetGameInputMode(EGameInputMode::GameOnly);
}

#undef LOCTEXT_NAMESPACE
