// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "ElectricActor.generated.h"

UCLASS(Abstract)
class FAF_REV_API AElectricActorBase : public AActor
{
	GENERATED_BODY()

public:

	AElectricActorBase();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<USceneComponent> SceneRoot;

	UFUNCTION(BlueprintCallable, Category = "ElectricActor")
		void AddEnergy(const FName Key, const uint8 Value);

	UFUNCTION(BlueprintCallable, Category = "ElectricActor")
		void RemoveEnergy(const FName Key);

	UFUNCTION(BlueprintPure, Category = "ElectricActor")
		uint8 GetEnergy();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Energy Changed")
		void EnergyChangedEvent(const uint8 Total);
	
protected:
	
	TMap<FName, uint8> Energy;
	TPair<bool, uint8> CachedEnergy;
	
	virtual void OnEnergyChanged(const uint8 Total);
};
