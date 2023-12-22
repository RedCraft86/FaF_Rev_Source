// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "CCTVCamera.generated.h"

UCLASS()
class GAMECORE_API ACCTVCamera : public AActor
{
	GENERATED_BODY()

public:

	ACCTVCamera();

	bool IsOnCamera(const AActor* Actor);

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
};
