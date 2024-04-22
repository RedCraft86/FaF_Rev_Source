// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRGameMode.h"
#include "FRController.h"
#include "GTUserWidget.h"
#include "FRPlayer.h"

AFRGameModeBase::AFRGameModeBase()
{
	PlayerControllerClass = AFRControllerBase::StaticClass();
	DefaultPawnClass = AFRPlayerBase::StaticClass();
}

UGTUserWidget* AFRGameModeBase::FindOrAddWidget(const TSubclassOf<UGTUserWidget> Class, const FGameplayTagContainer InTags)
{
	UGTUserWidget** ObjPtr = WidgetObjs.Find(Class);
	if (ObjPtr && *ObjPtr)
	{
		(*ObjPtr)->WidgetTags.AppendTags(InTags);
		return *ObjPtr;
	}
	
	UGTUserWidget* Obj = UGTUserWidget::CreateSmartWidget(FRController(this), Class, InTags);
	if (Obj) WidgetObjs.Add(Class, Obj);
	return Obj;
}

UGTUserWidget* AFRGameModeBase::GetWidget(const TSubclassOf<UGTUserWidget> Class, const FGameplayTag FilterTag)
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

void AFRGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	for (const TPair<TSubclassOf<UGTUserWidget>, FGameplayTagContainer>& Pair : DefaultWidgets)
	{
		FindOrAddWidget(Pair.Key, Pair.Value);
	}
}
