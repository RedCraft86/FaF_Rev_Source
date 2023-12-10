// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "InstancedStruct.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RCUtilsSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRCGlobalEventMulti, UObject*, Caller, FInstancedStruct, Payload);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FRCGlobalEventSingle, UObject*, Caller, FInstancedStruct, Payload);

UCLASS(DisplayName = "RCUtils Subsystem")
class RCRUNTIME_API URCUtilsSubsystem final : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	URCUtilsSubsystem();
	
	static URCUtilsSubsystem* Get(const UObject* WorldContext);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void SetGlobalVariable(const FGameplayTag& InKey, const FInstancedStruct& InStruct, bool bLevelVariable = false);
	FInstancedStruct GetGlobalVariable(const FGameplayTag& InKey) const;
	void RemoveGlobalVariable(const FGameplayTag& InKey);
	void RemoveAllGlobalVariables();

	void BindToGlobalEvent(const FGameplayTag& InKey, const FRCGlobalEventSingle& InDelegate);
	void UnbindFromGlobalEvent(const FGameplayTag& InKey, const FRCGlobalEventSingle& InDelegate);
	void InvokeGlobalEvent(const FGameplayTag& InKey, UObject* Caller, const FInstancedStruct& InPayload) const;
	void RemoveGlobalEvent(const FGameplayTag& InKey);
	void RemoveAllGlobalEvents();

private:

	void PostLoadMap(UWorld* World);

	TArray<FGameplayTag> LevelVariables;
	TMap<FGameplayTag, FInstancedStruct> GlobalVariables;
	TMap<FGameplayTag, FRCGlobalEventMulti> GlobalEvents;
};
