// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCQuickTimeEventWidget.h"
#include "Player/GCQuickTimeEventManager.h"
#include "Components/PanelWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UGCQuickTimeKeyWidget::Setup(UGCQuickTimeEventManager* InManager, const FString& InID)
{
	Manager = InManager;
	KeyID = InID;
	Key = Manager->GetKeyFromID(KeyID);

	KeyText->SetText(FText::FromName(Key.GetFName()));
}

void UGCQuickTimeEventWidget::Setup(UGCQuickTimeEventManager* InManager)
{
	Manager = InManager;
	Manager->OnKeyAdded.AddDynamic(this, &UGCQuickTimeEventWidget::OnKeyAdd);
	Manager->OnKeyAdded.AddDynamic(this, &UGCQuickTimeEventWidget::OnKeyRemove);
}

void UGCQuickTimeEventWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (IsInViewport())
	{
		FillBar->SetPercent(Manager->GetPercent());
	}
}

void UGCQuickTimeEventWidget::NativeDestruct()
{
	Super::NativeDestruct();
	FillBar->SetPercent(0.5f);
	KeySlots->ClearChildren();
	KeyWidgetMap.Empty();
}

void UGCQuickTimeEventWidget::OnKeyAdd(const FString& InID)
{
	UGCQuickTimeKeyWidget* Widget = CreateWidget<UGCQuickTimeKeyWidget>(this, KeyWidgetClass);
	Widget->Setup(Manager, InID);
	KeyWidgetMap.Add(InID, Widget);
	KeySlots->AddChild(Widget);
}

void UGCQuickTimeEventWidget::OnKeyRemove(const FString& InID)
{
	UGCQuickTimeKeyWidget** WidgetPtrPtr = KeyWidgetMap.Find(InID);
	if (WidgetPtrPtr && *WidgetPtrPtr)
	{
		if (Manager->WasKeySuccess(InID))
		{
			(*WidgetPtrPtr)->OnSuccess();
		}
		else if (Manager->WasKeyFailed(InID))
		{
			(*WidgetPtrPtr)->OnFailed();
		}
		else
		{
			(*WidgetPtrPtr)->RemoveFromParent();
		}
	}

	KeyWidgetMap.Remove(InID);
}
