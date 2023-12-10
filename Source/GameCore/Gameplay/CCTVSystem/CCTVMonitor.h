// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "CCTVMonitor.generated.h"

UCLASS()
class GAMECORE_API ACCTVMonitor : public AActor
{
	GENERATED_BODY()

public:

	ACCTVMonitor();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
};
