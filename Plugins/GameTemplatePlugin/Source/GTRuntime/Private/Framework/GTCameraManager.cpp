// Copyright (C) RedCraft86. All Rights Reserved.

#include "GTCameraManager.h"
#include "Kismet/GameplayStatics.h"

AGTCameraManager::AGTCameraManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AGTCameraManager::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->bIsCameraMoveableWhenPaused = true;
}

void AGTCameraManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (GetWorld()->IsPaused() && GetTickableWhenPaused())
	{
		UpdateCamera(DeltaSeconds);
	}
}

/* Statics */

AGTCameraManager* AGTCameraManager::GetCameraManagerSmart(const UObject* WorldContextObject, const TSubclassOf<AGTCameraManager> Class)
{
	AGTCameraManager* Obj = Cast<AGTCameraManager>(UGameplayStatics::GetPlayerCameraManager(WorldContextObject, 0));
	return Obj && Obj->IsA(Class) ? Obj : nullptr;
}

EGTValidPins AGTCameraManager::GetCameraManagerChecked(AGTCameraManager*& OutObject, const UObject* WorldContextObject, const TSubclassOf<AGTCameraManager> Class)
{
	OutObject = GetCameraManagerSmart(WorldContextObject, Class);
	return IsValid(OutObject) ? EGTValidPins::Valid : EGTValidPins::NotValid;
}
