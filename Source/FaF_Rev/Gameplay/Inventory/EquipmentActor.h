// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "EquipmentActor.generated.h"

UCLASS(Abstract)
class FAF_REV_API AEquipmentActor : public AActor
{
	GENERATED_BODY()

public:

	AEquipmentActor()
	{
		PrimaryActorTick.bCanEverTick = true;
		PrimaryActorTick.bStartWithTickEnabled = false;

		SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
		SetRootComponent(SceneRoot);
	}

	UPROPERTY(VisibleAnywhere, Category = "Subobject")
	TObjectPtr<USceneComponent> SceneRoot;

	virtual void OnUse() {}
	virtual void OnStartAltUse() {}
	virtual void OnEndAltUse() {}
};
