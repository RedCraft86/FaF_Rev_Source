// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTActor.h"
#include "ElectricActor.generated.h"

UCLASS(Abstract)
class FAF_REV_API AElectricActorBase : public AGTActor
{
	GENERATED_BODY()

public:

	AElectricActorBase();

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
	
	virtual void OnEnergyChanged();
	virtual void OnStateChanged(const bool bInState);
	
	virtual void BeginPlay() override;
	virtual void OnEnableStateChanged(const bool bIsEnabled) override;
};
