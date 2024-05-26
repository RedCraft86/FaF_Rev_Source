// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "Engine/LevelScriptActor.h"
#include "LevelLoadInterface.generated.h"

UINTERFACE()
class ULevelLoadInterface : public UInterface
{
	GENERATED_BODY()
};

class FAF_REV_API ILevelLoadInterface
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent, Category = "LevelLoadInterface")
		void OnLoadLevel();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "LevelLoadInterface")
		void OnUnloadLevel();
	
	static void Load(ALevelScriptActor* Target)
	{
		if (IsValid(Target) && Target->Implements<ULevelLoadInterface>())
		{
			ILevelLoadInterface::Execute_OnLoadLevel(Target);
		}
	}

	static void Unload(ALevelScriptActor* Target)
	{
		if (IsValid(Target) && Target->Implements<ULevelLoadInterface>())
		{
			ILevelLoadInterface::Execute_OnUnloadLevel(Target);
		}
	}
};
