﻿// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRGameMode.h"
#include "FRPlayerController.h"
#include "GTUserWidget.h"
#include "FRPlayer.h"

AFRGameModeBase::AFRGameModeBase(): PhotoModeActor(nullptr), InspectionActor(nullptr)
{
	PlayerControllerClass = AFRPlayerController::StaticClass();
	DefaultPawnClass = AFRPlayerBase::StaticClass();
}

EFoundPins AFRGameModeBase::GetGameWidget(UGTUserWidget*& FoundWidget, const UObject* WorldContextObject, const TSubclassOf<UGTUserWidget> Class, const FGameplayTag FilterTag)
{
	AFRGameModeBase* GM = FRGamemode(WorldContextObject);
	FoundWidget = GM ? GM->GetWidget(Class, FilterTag) : nullptr;
	return FoundWidget ? EFoundPins::Found : EFoundPins::NotFound;
}
