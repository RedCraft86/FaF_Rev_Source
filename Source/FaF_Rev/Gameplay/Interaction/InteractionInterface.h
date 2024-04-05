// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

class AFRPlayer;

#define DefaultInteractionLabel NSLOCTEXT("Interaction", "GenericLabel", "Interact");

UINTERFACE()
class UInteractionInterface final : public UInterface
{
	GENERATED_BODY()
};

class FAF_REV_API IInteractionInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction", DisplayName = "Begin Interact")
		void OnBeginInteract(AFRPlayer* Player, const FHitResult& HitResult);
	virtual void OnBeginInteract_Implementation(AFRPlayer* Player, const FHitResult& HitResult) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction", DisplayName = "End Interact")
		void OnEndInteract(AFRPlayer* Player);
	virtual void OnEndInteract_Implementation(AFRPlayer* Player) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction", DisplayName = "Begin AI Interact")
		void OnBeginAIInteract(APawn* Character, const FHitResult& HitResult);
	virtual void OnBeginAIInteract_Implementation(APawn* Character, const FHitResult& HitResult) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		bool GetInteractionInfo(FText& DisplayName);
	virtual bool GetInteractionInfo_Implementation(FText& DisplayName)
	{
		DisplayName = FText::GetEmpty();
		return false;
	}
};

namespace Interaction
{
	static bool ImplementedBy(const UObject* Target) 
	{ 
		return IsValid(Target) ? Target->Implements<UInteractionInterface>() : false; 
	} 
	
	static void OnBeginInteract(UObject* Target, AFRPlayer* Player, const FHitResult& HitResult)
	{
		if (ImplementedBy(Target))
		{
			IInteractionInterface::Execute_OnBeginInteract(Target, Player, HitResult);
		}
	}
	
	static void OnEndInteract(UObject* Target, AFRPlayer* Player)
	{
		if (ImplementedBy(Target))
		{
			IInteractionInterface::Execute_OnEndInteract(Target, Player);
		}
	}

	static void OnBeginAIInteract(UObject* Target, APawn* Character, const FHitResult& HitResult)
	{
		if (ImplementedBy(Target))
		{
			IInteractionInterface::Execute_OnBeginAIInteract(Target, Character, HitResult);
		}
	}

	static bool GetInteractionInfo(UObject* Target, FText& DisplayName)
	{
		if (ImplementedBy(Target))
		{
			return IInteractionInterface::Execute_GetInteractionInfo(Target, DisplayName);
		}
		
		DisplayName = DefaultInteractionLabel;
		return false;
	}
}
