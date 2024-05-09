// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTGameMode.h"
#include "FRGameMode.generated.h"

#define FRGameMode(Context) AGTGameModeBase::Get<AFRGameModeBase>(Context)

UCLASS(Abstract)
class FAF_REV_API AFRGameModeBase final : public AGTGameModeBase
{
	GENERATED_BODY()

	friend class AFRPlayerBase;

public:

	AFRGameModeBase();

	UPROPERTY(Transient)
		class AInspectionActor* InspectionActor;
};
