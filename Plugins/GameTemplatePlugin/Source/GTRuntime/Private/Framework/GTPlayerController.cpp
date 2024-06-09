// Copyright (C) RedCraft86. All Rights Reserved.

#include "GTPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GTCameraManager.h"

AGTPlayerController::AGTPlayerController()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
	
	PlayerCameraManagerClass = AGTCameraManager::StaticClass();
}

/* Statics */

AGTPlayerController* AGTPlayerController::GetPlayerControllerSmart(const UObject* WorldContextObject, const TSubclassOf<AGTPlayerController> Class)
{
	AGTPlayerController* Obj = Cast<AGTPlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, 0));
	return Obj && Obj->IsA(Class) ? Obj : nullptr;
}

EGTValidPins AGTPlayerController::GetPlayerControllerChecked(AGTPlayerController*& OutObject, const UObject* WorldContextObject, const TSubclassOf<AGTPlayerController> Class)
{
	OutObject = GetPlayerControllerSmart(WorldContextObject, Class);
	return IsValid(OutObject) ? EGTValidPins::Valid : EGTValidPins::NotValid;
}
