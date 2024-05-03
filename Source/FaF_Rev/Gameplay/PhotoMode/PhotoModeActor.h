// Copyright (C) Tayzar L. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "PhotoModeActor.generated.h"

UCLASS(Abstract)
class FAF_REV_API APhotoModeActor final : public AActor
{
	GENERATED_BODY()

public:

	APhotoModeActor();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
};
