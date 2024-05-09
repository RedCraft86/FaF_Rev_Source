// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "WidgetInterface.generated.h"

UINTERFACE()
class UWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class FAF_REV_API IWidgetInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ExitInterface")
		void Return(UUserWidget* From);
	virtual void Return_Implementation(UUserWidget* From) {}

	static void Return(UObject* Target, UUserWidget* From)
	{
		if (IsValid(Target) && Target->Implements<UWidgetInterface>())
		{
			IWidgetInterface::Execute_Return(Target, From);
		}
	}
};