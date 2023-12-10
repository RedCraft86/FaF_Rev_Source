// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "GCSequenceInterface.generated.h"

UINTERFACE(BlueprintType, DisplayName = "Game Sequence Interface")
class UGCSequenceInterface : public UInterface
{
	GENERATED_BODY()
};

class GAMECORE_API IGCSequenceInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface|SequenceWorld", DisplayName = "Load World")
		void OnLoadWorld();
	virtual void OnLoadWorld_Implementation() {}
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface|SequenceWorld", DisplayName = "Unload World")
    	void OnUnloadWorld();
    virtual void OnUnloadWorld_Implementation() {}
};

namespace GCSequence
{
	GAMECORE_API FORCEINLINE bool ImplementedBy(const UObject* Target) 
	{ 
		return IsValid(Target) ? Target->Implements<UGCSequenceInterface>() : false; 
	}
	
	GAMECORE_API FORCEINLINE void OnLoadWorld(UObject* Target)
    {
    	if (ImplementedBy(Target))
    	{
            IGCSequenceInterface::Execute_OnLoadWorld(Target);
    	}
    }
    
    GAMECORE_API FORCEINLINE void OnUnloadWorld(UObject* Target)
    {
        if (ImplementedBy(Target))
        {
        	IGCSequenceInterface::Execute_OnUnloadWorld(Target);
        }
    }
}