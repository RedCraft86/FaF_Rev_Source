// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTPlayerController.h"
#include "FRController.generated.h"

#define FRController(Context) AGTPlayerController::Get<AFRControllerBase>(Context)

UCLASS(Abstract)
class FAF_REV_API AFRControllerBase final : public AGTPlayerController
{
	GENERATED_BODY()
};
