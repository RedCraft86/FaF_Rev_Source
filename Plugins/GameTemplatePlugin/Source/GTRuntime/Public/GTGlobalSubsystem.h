// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameplayTags.h"
#include "InstancedStruct.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GTGlobalSubsystem.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FGTGlobalEventSingle, UObject*, Instigator, const FInstancedStruct&, Payload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGTGlobalEventMulti, UObject*, Instigator, const FInstancedStruct&, Payload);

UCLASS(BlueprintType, DisplayName = "Global Subsystem")
class GTRUNTIME_API UGTGlobalSubsystem final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "GameGlobal", meta = (AdvancedDisplay = "bUntilMapChange"))
		void SetGlobalVariable(const FGameplayTag Tag, const FInstancedStruct& Data, const bool bUntilMapChange = false);
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "GameGlobal")
		FInstancedStruct GetGlobalVariable(const FGameplayTag Tag) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "GameGlobal")
		void DeleteGlobalVariable(const FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "GameGlobal", meta = (DefaultToSelf = "Instigator"))
		void InvokeGlobalEvent(const FGameplayTag Tag, UObject* Instigator, const FInstancedStruct& Data);

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "GameGlobal")
		void BindGlobalEvent(const FGameplayTag Tag, FGTGlobalEventSingle Callback);
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "GameGlobal")
		void UnbindGlobalEvent(const FGameplayTag Tag, const FGTGlobalEventSingle& Callback);

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "GameGlobal")
		void DeleteGlobalEvent(const FGameplayTag Tag);
	
private:

	TSet<FGameplayTag> MapVariables;
	TMap<FGameplayTag, FInstancedStruct> Variables;
	TMap<FGameplayTag, FGTGlobalEventMulti> Events;

	void PostLoadMap(UWorld* World);
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public: // Statics
	
	static UGTGlobalSubsystem* Get(const UObject* WorldContext)
	{
		const UWorld* World = WorldContext ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
		const UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
		return GI ? GI->GetSubsystem<UGTGlobalSubsystem>() : nullptr;
	}
};
