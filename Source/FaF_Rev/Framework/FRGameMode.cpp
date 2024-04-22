// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRGameMode.h"
#include "FRController.h"
#include "GTUserWidget.h"
#include "FRPlayer.h"

AFRGameMode::AFRGameMode()
{
	PlayerControllerClass = AFRController::StaticClass();
	DefaultPawnClass = AFRPlayer::StaticClass();
}

UGTUserWidget* AFRGameMode::FindOrAddWidget(const TSubclassOf<UGTUserWidget> Class, const FGameplayTagContainer Tags)
{
	UGTUserWidget** ObjPtr = WidgetObjs.Find(Class);
	if (ObjPtr && *ObjPtr)
	{
		(*ObjPtr)->WidgetTags.AppendTags(Tags);
		return *ObjPtr;
	}
	
	UGTUserWidget* Obj = UGTUserWidget::CreateSmartWidget(FRController(this), Class, Tags);
	if (Obj) WidgetObjs.Add(Class, Obj);
	return Obj;
}

UGTUserWidget* AFRGameMode::GetWidget(const TSubclassOf<UGTUserWidget> Class, const FGameplayTag FilterTag)
{
	WidgetObjs.Remove(nullptr);
	if (UGTUserWidget** ObjPtr = WidgetObjs.Find(Class))
	{
		if (ObjPtr && *ObjPtr)
		{
			if (FilterTag.IsValid())
			{
				return (*ObjPtr)->WidgetTags.HasTagExact(FilterTag) ? *ObjPtr : nullptr;
			}
			
			return *ObjPtr;
		}
	}
	
	return nullptr;
}

void AFRGameMode::BeginPlay()
{
	Super::BeginPlay();
	for (const TSubclassOf<UGTUserWidget> WidgetClass : DefaultWidgets)
	{
		FindOrAddWidget(WidgetClass, {});
	}
}
