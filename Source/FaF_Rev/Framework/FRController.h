// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTPlayerController.h"
#include "FRController.generated.h"

#define GET_CONTROLLER(Context) AFRController::Get<AFRController>(Context)

UCLASS()
class FAF_REV_API AFRController final : public AGTPlayerController
{
	GENERATED_BODY()
};
