// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Optimization/SmartCulling.h"
#include "ElectricActor.generated.h"

UCLASS(Abstract)
class FAF_REV_API AElectricActorBase : public AActor
{
	GENERATED_BODY()

public:

	AElectricActorBase();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<USmartCullingComponent> SmartCulling;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (DisplayPriority = -1))
		uint8 MinEnergy;

	UFUNCTION(BlueprintCallable, Category = "ElectricActor")
		void AddEnergy(const FName Key, const uint8 Value);

	UFUNCTION(BlueprintCallable, Category = "ElectricActor")
		void RemoveEnergy(const FName Key);

	UFUNCTION(BlueprintPure, Category = "ElectricActor")
		uint8 GetEnergy();

	UFUNCTION(BlueprintPure, Category = "ElectricActor")
		bool GetState();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Energy Changed")
		void EnergyChangedEvent(const uint8 Total);
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "State Changed")
		void StateChangedEvent(const bool bState);
	
protected:

	bool bCachedState;
	TMap<FName, uint8> Energy;
	TPair<bool, uint8> CachedEnergy;
	
	virtual void OnEnergyChanged(const uint8 Total);
	virtual void OnStateChanged(const bool bState);
	virtual void BeginPlay() override;
};
