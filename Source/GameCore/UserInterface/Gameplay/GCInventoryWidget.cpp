// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "GCInventoryWidget.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Components/RichTextBlock.h"
#include "Player/GCPlayerController.h"
#include "Inventory/GCInspectionActor.h"
#include "Inventory/GCInventoryManager.h"
#include "UserInterface/Gameplay/GCGameplayWidget.h"
#include "UserInterface/Gameplay/GCNarrativeWidget.h"

UGCInventorySlotWidget::UGCInventorySlotWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	BackgroundImage = nullptr;
	IconImage = nullptr;
	IconEquip = nullptr;
	IconData = nullptr;
	AmountText = nullptr;
	SlotButton = nullptr;

	NormalOutlineColor = FLinearColor(0.025f, 0.025f, 0.025f, 1.0f);
	SelectedOutlineColor = FLinearColor(0.75f, 0.75f, 0.0f, 1.0f);
	ItemID = NAME_None;
	SlotData = {};
	bEquipped = false;
}

void UGCInventorySlotWidget::InitData()
{
	const FGCInvItemData ItemData = UGCInventoryManager::GetItemData(ItemID);
	IconImage->SetBrushFromTexture(ItemData.IconImage.LoadSynchronous(), false);
	IconEquip->SetVisibility(bEquipped ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	IconData->SetVisibility(SlotData.Binary.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
	AmountText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), SlotData.Amount, ItemData.MaxStackSize)));

	SlotButton->SetToolTipText(ItemData.DisplayName);
	SlotButton->OnClicked.AddDynamic(this, &UGCInventorySlotWidget::OnClicked);
}

void UGCInventorySlotWidget::MarkSelected(const bool bSelected) const
{
	BackgroundImage->SetColorAndOpacity(bSelected ? SelectedOutlineColor : NormalOutlineColor);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGCInventorySlotWidget::OnClicked()
{
	ParentUI->SelectSlot(ItemID);
}

UGCInventoryWidget::UGCInventoryWidget(const FObjectInitializer& ObjectInitializer)
	: UGCUserWidget(ObjectInitializer)
{
	WidgetZOrder = 12;
	bAutoAddToViewport = false;
	
	SlotsBox = nullptr;
	ItemPreviewImage = nullptr;
	ItemTitleText = nullptr;
	ItemDescText = nullptr;
	ItemTypeText = nullptr;
	EquipStateBox = nullptr;
	SlotsFadeAnim = nullptr;
	DescFadeAnim = nullptr;
	UsageButton = nullptr;
	UsageText = nullptr;
	ViewingTitle = nullptr;
	ViewContentBox = nullptr;
	ViewContentImage = nullptr;
	ReadContentBox = nullptr;
	ReadContentText = nullptr;
	ViewContentImage = nullptr;
	FinishViewingButton = nullptr;

	SlotWidgetClass = NULL;
	ImageDescHeight = 200.0f;
	EquipItemLabel = FText::FromString("Equip");
	UnequipItemLabel = FText::FromString("Unequip");
	ViewItemLabel = FText::FromString("Transcribe/View");
	
	InventoryManager = nullptr;
	SlotWidgets = {};
	SelectedItem = NAME_None;
}

void UGCInventoryWidget::OnInventoryUpdate()
{
	if (InventoryManager && SlotsBox && SlotWidgetClass)
	{
		const TArray<FName> ItemIDs = InventoryManager->GetSortedItemIDs();
		const FGCInvEquipmentInfo Equipment = InventoryManager->GetActiveEquipment();
		if (SlotWidgets.IsEmpty() && ItemIDs.IsEmpty()) // Only do nothing if update is FROM Empty TO Empty
		{
			return;
		}
		
		SlotWidgets.Empty();
		SlotsBox->ClearChildren();
	    for (const FName& ID : ItemIDs)
	    {
	    	if (InventoryManager->HaveItemSlot(ID))
	    	{
	    		const FGCInvItemSlotData SlotData = InventoryManager->GetItemSlot(ID);
	    		UGCInventorySlotWidget* SlotWidget = CreateWidget<UGCInventorySlotWidget>(this, SlotWidgetClass);
	    		SlotWidget->ParentUI = this;
	    		SlotWidget->ItemID = ID;
	    		SlotWidget->SlotData = SlotData;
	    		SlotWidget->bEquipped = Equipment.ItemID == ID;
	    		SlotWidget->InitData();

	    		SlotWidgets.Add(ID, SlotWidget);
	    		SlotsBox->AddChild(SlotWidget);
	    	}
	    }

		GetWorld()->GetTimerManager().SetTimerForNextTick([this, ItemIDs](){
			if (ItemIDs.IsEmpty())
			{
				SelectSlot(NAME_None);
			}
			else if (!ItemIDs.Contains(SelectedItem))
			{
				SelectSlot(ItemIDs[0]);
			}
			else if (const UGCInventorySlotWidget* Slot = SlotWidgets.FindRef(SelectedItem))
			{
				UpdateItemInfo();
				Slot->MarkSelected(true);
			}
		});
		
		PlayAnimation(SlotsFadeAnim);
	}
}

void UGCInventoryWidget::UpdateItemInfo()
{
	PLAY_FIRST_FRAME(DescFadeAnim);

	ItemPreviewImage->SetVisibility(ESlateVisibility::Collapsed);
	ItemTitleText->SetText(FText::FromString(TEXT("None Selected")));
	if (ItemDescText)
	{
		ItemDescText->SetText(FText::GetEmpty());
	}
	else
	{
		SetItemDescText(FText::GetEmpty());
	}
	ItemTypeText->SetText(FText::GetEmpty());
	EquipStateBox->SetVisibility(ESlateVisibility::Collapsed);
	UsageButton->SetVisibility(ESlateVisibility::Collapsed);
	UsageText->SetText(FText::GetEmpty());
	
	if (InventoryManager && !SelectedItem.IsNone() && SlotWidgets.Contains(SelectedItem))
	{
		const FGCInvItemData Data = UGCInventoryManager::GetItemData(SelectedItem);
		const bool bEquipped = InventoryManager->GetActiveEquipment().ItemID == SelectedItem;
		
		ItemTitleText->SetText(Data.DisplayName);
		if (ItemDescText)
		{
			ItemDescText->SetText(Data.Description);
		}
		else
		{
			SetItemDescText(Data.Description);
		}
		ItemTypeText->SetText(FText::FromString(TEXT("Type: ") + Data.GetTypeString()));
		
		if (bEquipped)
		{
			EquipStateBox->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		
		switch (Data.ItemType)
		{
		case EGCInvItemType::Consumable:
			if (Data.ConsumableClass != NULL)
			{
				UsageButton->SetVisibility(ESlateVisibility::Visible);
				UsageText->SetText(Data.UseDisplayText);
			}
			break;
			
		case EGCInvItemType::Equipment:
			if (Data.EquipmentClass != NULL)
			{
				UsageButton->SetVisibility(ESlateVisibility::Visible);
				UsageText->SetText(bEquipped ? UnequipItemLabel : EquipItemLabel);
			}
			break;

		case EGCInvItemType::Viewable:
			if (Data.ViewImage || !Data.ViewText.IsEmptyOrWhitespace())
			{
				UsageButton->SetVisibility(ESlateVisibility::Visible);
				UsageText->SetText(ViewItemLabel);
			}
			break;

		default: break;
		}

		if (InspectionActor)
		{
			ItemPreviewImage->SetVisibility(ESlateVisibility::HitTestInvisible);
			InspectionActor->SetPreviewItemID(SelectedItem);
			InspectionActor->SetInspectionPaused(false);
		}
	}
	else if (InspectionActor)
	{
		InspectionActor->SetInspectionPaused(true);
	}
	
	PlayAnimation(DescFadeAnim);
}

void UGCInventoryWidget::SelectSlot(const FName& ItemID)
{
	if (SelectedItem == ItemID)
	{
		return;
	}

	SelectedItem = NAME_None;
	if (SlotWidgets.Contains(ItemID))
	{
		SelectedItem = ItemID;
		for (const TPair<FName, UGCInventorySlotWidget*>& SlotWidget : SlotWidgets)
		{
			if (!SlotWidget.Value)
				continue;
			
			if (SlotWidget.Key == ItemID)
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

void UGCInventoryWidget::InitData(AGCPlayerController* Controller)
{
	Super::InitData(Controller);
	InventoryManager = Controller->GetInventoryManager();
	InspectionActor = Controller->GetInspectionActor();

	UsageButton->OnClicked.AddDynamic(this, &UGCInventoryWidget::OnUsageClicked);
	FinishViewingButton->OnClicked.AddDynamic(this, &UGCInventoryWidget::OnReadFinishClicked);
}

void UGCInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	PlayerController->GetUserWidget<UGCGameplayWidget>()->SetWidgetHidden(true);
	PlayerController->GetUserWidget<UGCNarrativeWidget>()->SetWidgetHidden(true);
	if (InspectionActor)
	{
		InspectionActor->BeginInspection();
	}
	
	PLAY_FIRST_FRAME(ViewingFadeAnim);
	SlotsBox->ClearChildren();
	UpdateItemInfo();
	
	if (InventoryManager)
	{
		InventoryManager->OnUpdate.RemoveAll(this);
		InventoryManager->OnUpdate.AddUObject(this, &UGCInventoryWidget::OnInventoryUpdate);
		OnInventoryUpdate();
	}
}

void UGCInventoryWidget::NativeDestruct()
{
	Super::NativeDestruct();
	PlayerController->GetUserWidget<UGCGameplayWidget>()->SetWidgetHidden(false);
	PlayerController->GetUserWidget<UGCNarrativeWidget>()->SetWidgetHidden(false);
	if (InspectionActor)
	{
		InspectionActor->EndInspection();
	}
	
	if (InventoryManager)
	{
		InventoryManager->OnUpdate.RemoveAll(this);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGCInventoryWidget::OnUsageClicked()
{
	if (InventoryManager && !SelectedItem.IsNone() && SlotWidgets.Contains(SelectedItem))
	{
		const FGCInvItemData Data = UGCInventoryManager::GetItemData(SelectedItem);
		
		switch (Data.ItemType)
		{
		case EGCInvItemType::Consumable:
			InventoryManager->ConsumeItem(SelectedItem);
			break;
			
		case EGCInvItemType::Equipment:
			if (InventoryManager->GetActiveEquipment().ItemID == SelectedItem)
			{
				InventoryManager->UnequipItem();
			}
			else
			{
				InventoryManager->EquipItem(SelectedItem);
			}
			break;

		case EGCInvItemType::Viewable:
			ViewingTitle->SetText(Data.DisplayName);
			
			ViewContentImage->SetBrushFromTexture(Data.ViewImage.LoadSynchronous());
			ViewContentBox->SetVisibility(IsValid(ViewContentImage->GetBrush().GetResourceObject()) ?
				ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

			if (ReadContentText)
			{
				ReadContentText->SetText(Data.ViewText);
			}
			else
			{
				SetReadContentText(Data.ViewText);
			}
			ReadContentBox->SetVisibility(Data.ViewText.IsEmptyOrWhitespace() ?
				ESlateVisibility::Collapsed : ESlateVisibility::Visible);
			
			if (Data.ViewImage && !Data.ViewText.IsEmptyOrWhitespace())
			{
				ReadContentBox->SetHeightOverride(ImageDescHeight);
			}
			else
			{
				ReadContentBox->ClearHeightOverride();
			}
			
			PlayAnimation(ViewingFadeAnim);
			break;

		default: break;
		}
	}
}

void UGCInventoryWidget::OnReadFinishClicked()
{
	PlayAnimationReverse(ViewingFadeAnim);
}
