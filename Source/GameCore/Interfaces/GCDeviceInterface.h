// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

// ReSharper disable CppUE4BlueprintCallableFunctionMayBeStatic

#pragma once

#include "UObject/Interface.h"
#include "GCDeviceInterface.generated.h"

UINTERFACE(BlueprintType, DisplayName = "Device Interface")
class UGCDeviceInterface : public UInterface
{
	GENERATED_BODY()
};

class GAMECORE_API IGCDeviceInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface|Device")
		void ForceExitDevice(const FString& Reason);
	virtual void ForceExitDevice_Implementation(const FString& Reason) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface|Device")
		TArray<UAudioComponent*> GetAudioSources() const;
	virtual TArray<UAudioComponent*> GetAudioSources_Implementation() const { return {}; }
};

namespace GCDeviceInterface
{
	GAMECORE_API FORCEINLINE bool ImplementedBy(const UObject* Target) 
	{ 
		return IsValid(Target) ? Target->Implements<UGCDeviceInterface>() : false; 
	} 
	
	GAMECORE_API FORCEINLINE void ForceExitDevice(UObject* Target, const FString& Reason = TEXT("Unknown"))
	{
		if (ImplementedBy(Target))
		{
			IGCDeviceInterface::Execute_ForceExitDevice(Target, Reason);
		}
	}

	GAMECORE_API FORCEINLINE TArray<UAudioComponent*> GetAudioSources(const UObject* Target)
	{
		if (ImplementedBy(Target))
		{
			return IGCDeviceInterface::Execute_GetAudioSources(Target);
		}

		return {};
	}
}