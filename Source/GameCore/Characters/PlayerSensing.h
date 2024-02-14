// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "Components/ArrowComponent.h"
#include "PlayerSensing.generated.h"

UCLASS(ClassGroup = (AI), meta = (BlueprintSpawnableComponent))
class GAMECORE_API UPlayerSensing : public UArrowComponent
{
	GENERATED_BODY()

public:

	UPlayerSensing();

	UFUNCTION(EditAnywhere, Category = "SensingLogic")
		bool bDetectOnlyMovement;

	UFUNCTION(EditAnywhere, Category = "SensingLogic", meta = (EditCondition = "bDetectOnlyMovement"))
		float MinMovementSpeed;

protected:

	virtual void BeginPlay() override;

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
