// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "ForceExitInterface.generated.h"

UINTERFACE()
class UForceExitInterface : public UInterface
{
	GENERATED_BODY()
};

class FAF_REV_API IForceExitInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ForceExit")
		void ForceExit();
	virtual void ForceExit_Implementation() {}

	static void Exit(UObject* Target)
	{
		if (IsValid(Target) && Target->Implements<UForceExitInterface>())
		{
			IForceExitInterface::Execute_ForceExit(Target);
		}
	}
};