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

EFoundPins AFRGameModeBase::GetGameWidget(UGTUserWidget*& FoundWidget, const UObject* WorldContextObject, const TSubclassOf<UGTUserWidget> Class, const FGameplayTag FilterTag)
{
	AFRGameModeBase* GM = FRGamemode(WorldContextObject);
	FoundWidget = GM ? GM->GetWidget(Class, FilterTag) : nullptr;
	return FoundWidget ? EFoundPins::Found : EFoundPins::NotFound;
}
