// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "ConsumableActor.generated.h"

UCLASS()
class FAF_REV_API AConsumableActor final : public AActor
{
	GENERATED_BODY()

public:

	AConsumableActor()
	{
		PrimaryActorTick.bCanEverTick = true;
		PrimaryActorTick.bStartWithTickEnabled = false;

		SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
		SetRootComponent(SceneRoot);

		LifeSpan = 0.25f;
	}

	UPROPERTY(VisibleAnywhere, Category = "Subobject")
		TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float LifeSpan;

	bool UseItem()
	{
		SetLifeSpan(LifeSpan);
		return OnItemUse();
	}

protected:

	UFUNCTION(BlueprintImplementableEvent)
		bool OnItemUse();
};
