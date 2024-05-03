// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "InspectionActor.generated.h"

UCLASS(Abstract)
class FAF_REV_API AInspectionActor final : public AActor
{
	GENERATED_BODY()

public:

	AInspectionActor();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
};
