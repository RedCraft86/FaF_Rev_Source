// Copyright (C) Tayzar Linn. All Rights Reserved.

// ReSharper disable CppUE4BlueprintCallableFunctionMayBeStatic

#pragma once

#include "UObject/Interface.h"
#include "GCInteractionInterface.generated.h"

class AGCPlayerCharacter;

USTRUCT(BlueprintType, DisplayName = "Interaction Data")
struct GAMECORE_API FGCInteractionData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InteractionData")
		AActor* Actor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InteractionData")
		FText DisplayName;

	FGCInteractionData() : Actor(nullptr), DisplayName(FText::GetEmpty()) {}
	FGCInteractionData(AActor* InActor) : Actor(InActor) , DisplayName(FText::GetEmpty()) {}
	
	FORCEINLINE bool operator==(const FGCInteractionData& Other) const
	{
		return Actor == Other.Actor;
	}

	FORCEINLINE bool operator!=(const FGCInteractionData& Other) const
	{
		return Actor != Other.Actor;
	}

	void Reset()
	{
		Actor = nullptr;
		DisplayName = FText::GetEmpty();
	}

	bool IsValidData() const
	{
		return IsValid(Actor) && DisplayName.IsEmptyOrWhitespace();
	}
};

UINTERFACE(BlueprintType, DisplayName = "Interaction Interface")
class UGCInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class GAMECORE_API IGCInteractionInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface|Interaction", DisplayName = "Begin Interact")
		void OnBeginInteract(AGCPlayerCharacter* Player, const FHitResult& HitResult);
	virtual void OnBeginInteract_Implementation(AGCPlayerCharacter* Player, const FHitResult& HitResult) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface|Interaction", DisplayName = "End Interact")
		void OnEndInteract(AGCPlayerCharacter* Player);
	virtual void OnEndInteract_Implementation(AGCPlayerCharacter* Player) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface|Interaction", DisplayName = "Begin AI Interact")
		void OnBeginAIInteract(APawn* Character, const FHitResult& HitResult);
	virtual void OnBeginAIInteract_Implementation(APawn* Character, const FHitResult& HitResult) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface|Interaction")
		bool GetInteractionInfo(FText& DisplayName);
	virtual bool GetInteractionInfo_Implementation(FText& DisplayName)
	{
		DisplayName = FText::GetEmpty();
		return false;
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface|Interaction")
		bool IsTaskActor();
	virtual bool IsTaskActor_Implementation() { return false; }
};

namespace GCInteraction
{
	GAMECORE_API FORCEINLINE bool ImplementedBy(const UObject* Target) 
	{ 
		return IsValid(Target) ? Target->Implements<UGCInteractionInterface>() : false; 
	} 
	
	GAMECORE_API FORCEINLINE void OnBeginInteract(UObject* Target, AGCPlayerCharacter* Player, const FHitResult& HitResult)
	{
		if (ImplementedBy(Target))
		{
			IGCInteractionInterface::Execute_OnBeginInteract(Target, Player, HitResult);
		}
	}
	
	GAMECORE_API FORCEINLINE void OnEndInteract(UObject* Target, AGCPlayerCharacter* Player)
	{
		if (ImplementedBy(Target))
		{
			IGCInteractionInterface::Execute_OnEndInteract(Target, Player);
		}
	}

	GAMECORE_API FORCEINLINE void OnBeginAIInteract(UObject* Target, APawn* Character, const FHitResult& HitResult)
	{
		if (ImplementedBy(Target))
		{
			IGCInteractionInterface::Execute_OnBeginAIInteract(Target, Character, HitResult);
		}
	}

	GAMECORE_API FORCEINLINE bool GetInteractionInfo(UObject* Target, FText& DisplayName)
	{
		if (ImplementedBy(Target))
		{
			return IGCInteractionInterface::Execute_GetInteractionInfo(Target, DisplayName);
		}
		
		DisplayName = FText::GetEmpty();
		return false;
	}

	GAMECORE_API FORCEINLINE bool IsTaskActor(UObject* Target)
	{
		if (ImplementedBy(Target))
		{
			return IGCInteractionInterface::Execute_IsTaskActor(Target);
		}
		
		return false;
	}
}