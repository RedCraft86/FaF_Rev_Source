// Copyright (C) RedCraft86. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "InventoryWidget.h"
#include "Inventory/InventoryComponent.h"
#include "FRGameMode.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/TextBlock.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Inventory/InventoryItemData.h"

#define GET_INVENTORY GetGameMode<AFRGameModeBase>()->Inventory
#define LOCTEXT_NAMESPACE "Inventory"

void UInventorySlotWidgetBase::InitData(const FGuid& InSlotKey, const bool bEquipped)
{
	const FInventorySlotData& SlotData = ParentUI->GET_INVENTORY->GetInventory().FindRef(InSlotKey);
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

void UInventoryWidgetBase::RefreshUI()
{
	if (IsInViewport())
	{
		SlotKeys = GET_INVENTORY->GetSortedSlots();
	}
}

void UInventoryWidgetBase::SelectSlot(const FGuid& InKey)
{
}

void UInventoryWidgetBase::InitWidget()
{
	Super::InitWidget();
	GET_INVENTORY->OnUpdate.AddUObject(this, &UInventoryWidgetBase::RefreshUI);
}

void UInventoryWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshUI();
	GetGameMode<AFRGameModeBase>()->SetGameInputMode(EGameInputMode::GameAndUI, true,
		EMouseLockMode::LockAlways, false, this);

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
	GetGameMode<AFRGameModeBase>()->SetGameInputMode(EGameInputMode::GameOnly);
}

#undef LOCTEXT_NAMESPACE
