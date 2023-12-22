// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "GCCharacterInterface.generated.h"

UINTERFACE(BlueprintType, DisplayName = "Character Interface")
class UGCCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

class GAMECORE_API IGCCharacterInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface|Character", DisplayName = "Entered Dialogue")
		void OnEnteredDialogue();
	virtual void OnEnteredDialogue_Implementation() {}
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface|Character", DisplayName = "Exited Dialogue")
    	void OnExitedDialogue();
    virtual void OnExitedDialogue_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface|Character")
		FVector GetEyeWorldLocation();
	virtual FVector GetEyeWorldLocation_Implementation() { return FVector::ZeroVector; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface|Character")
		FVector GetEyeForwardVector();
	virtual FVector GetEyeForwardVector_Implementation() { return FVector::ForwardVector; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface|Character")
		USceneComponent* GetLookAtComponent();
	virtual USceneComponent* GetLookAtComponent_Implementation() { return nullptr; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface|Character")
		bool IsAnimatronic();
	virtual bool IsAnimatronic_Implementation() { return false; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface|Character")
		void RunNamedEvent(const FName& Name);
	virtual void RunNamedEvent_Implementation(const FName& Name) {}
};

namespace GCCharacter
{
	GAMECORE_API FORCEINLINE bool ImplementedBy(const UObject* Target) 
	{ 
		return IsValid(Target) ? Target->Implements<UGCCharacterInterface>() : false; 
	}
	
	GAMECORE_API FORCEINLINE void OnEnteredDialogue(UObject* Target)
    {
    	if (ImplementedBy(Target))
    	{
            IGCCharacterInterface::Execute_OnEnteredDialogue(Target);
    	}
    }
    
    GAMECORE_API FORCEINLINE void OnExitedDialogue(UObject* Target)
    {
        if (ImplementedBy(Target))
        {
        	IGCCharacterInterface::Execute_OnExitedDialogue(Target);
        }
    }

	GAMECORE_API FORCEINLINE FVector GetEyeWorldLocation(UObject* Target)
	{
		if (ImplementedBy(Target))
		{
			return IGCCharacterInterface::Execute_GetEyeWorldLocation(Target);
		}

		return FVector::ZeroVector;
	}

	GAMECORE_API FORCEINLINE FVector GetEyeForwardVector(UObject* Target)
	{
		if (ImplementedBy(Target))
		{
			return IGCCharacterInterface::Execute_GetEyeForwardVector(Target);
		}

		return FVector::ForwardVector;
	}

	GAMECORE_API FORCEINLINE USceneComponent* GetLookAtComponent(UObject* Target)
	{
		if (ImplementedBy(Target))
		{
			return IGCCharacterInterface::Execute_GetLookAtComponent(Target);
		}

		return nullptr;
	}

	GAMECORE_API FORCEINLINE bool IsAnimatronic(UObject* Target)
	{
		if (ImplementedBy(Target))
		{
			return IGCCharacterInterface::Execute_IsAnimatronic(Target);
		}

		return false;
	}

	GAMECORE_API FORCEINLINE void RunNamedEvent(const FName& Name, UObject* Target)
	{
		if (ImplementedBy(Target))
		{
			IGCCharacterInterface::Execute_RunNamedEvent(Target, Name);
		}
	}
}
