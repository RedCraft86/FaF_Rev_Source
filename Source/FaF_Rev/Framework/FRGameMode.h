﻿// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTGameMode.h"
#include "FRGameMode.generated.h"

#define FRGamemode(Context) AGTGameModeBase::Get<AFRGameModeBase>(Context)

UCLASS(Abstract)
class FAF_REV_API AFRGameModeBase final : public AGTGameModeBase
{
	GENERATED_BODY()

public:

	AFRGameModeBase();

	UPROPERTY(Transient)
		class APhotoModeActor* PhotoModeActor;

	UPROPERTY(Transient)
		class AInspectionActor* InspectionActor;
};
