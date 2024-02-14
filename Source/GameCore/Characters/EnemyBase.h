// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "Interfaces/GCCharacterInterface.h"
#include "EnemyBase.generated.h"

UCLASS(DisplayName = "Enemy AI Base")
class GAMECORE_API AEnemyBase : public ACharacter, public IGCCharacterInterface
{
	GENERATED_BODY()

public:

	AEnemyBase();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
