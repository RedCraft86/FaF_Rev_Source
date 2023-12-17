// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "NPCStaticBase.generated.h"

UCLASS()
class GAMECORE_API ANPCStaticBase final : public AActor
{
	GENERATED_BODY()

public:

	ANPCStaticBase();

protected:

	virtual void BeginPlay() override;


	virtual void Tick(float DeltaTime) override;
};
