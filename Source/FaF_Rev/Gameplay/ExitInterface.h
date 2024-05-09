// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "ExitInterface.generated.h"

UINTERFACE()
class UExitInterface : public UInterface
{
	GENERATED_BODY()
};

class FAF_REV_API IExitInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ExitInterface")
		void Exit();
	virtual void Exit_Implementation() {}

	static void Exit(UObject* Target)
	{
		if (IsValid(Target) && Target->Implements<UExitInterface>())
		{
			IExitInterface::Execute_Exit(Target);
		}
	}
};