// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "NPCBase.generated.h"

UCLASS()
class GAMECORE_API ANPCBase final : public ACharacter
{
	GENERATED_BODY()

public:
	
	ANPCBase();

protected:
	
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
};
