// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "InventoryInspectable.generated.h"

UCLASS()
class FAF_REV_API AInventoryInspectable final : public AActor
{
	GENERATED_BODY()

public:

	AInventoryInspectable();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
};
