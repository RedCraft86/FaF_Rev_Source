// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "GCElectricalTypes.generated.h"

UENUM(BlueprintType, DisplayName = "Condition Type")
enum class EGCConditionType : uint8
{
	RequiresAll,
	RequiresAny,
};

UENUM(BlueprintType, DisplayName = "Electronic Type")
enum class EGCElectronicType : uint8
{
	Generator,
	Connector,
	Toggle,
	Appliance
};
