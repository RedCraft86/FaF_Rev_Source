// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTActor.h"
#include "Interaction/InteractionInterface.h"
#include "ElectricSwitch.generated.h"

UCLASS(Abstract)
class FAF_REV_API AElectricSwitchBase final : public AGTActor, public IInteractionInterface
{
	GENERATED_BODY()

public:

	AElectricSwitchBase();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		bool bState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (ClampMin = 1, UIMin = 1))
		uint8 PowerValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		TSet<TObjectPtr<class AElectricActorBase>> Targets;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Settings")
		bool bSingleUse;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Settings")
		FInteractionInfo InteractionInfo;

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "State Changed")
		void StateChangedEvent(const bool bNewState);

	UFUNCTION(BlueprintCallable, Category = "ElectricSwitch")
		void SetState(const bool bNewState);

	UFUNCTION(BlueprintCallable, Category = "ElectricSwitch")
		void ToggleState();

protected:

	void UpdateStates();
	virtual bool GetInteractionInfo_Implementation(FInteractionInfo& Info) override;
	virtual void OnBeginInteract_Implementation(AFRPlayerBase* Player, const FHitResult& HitResult) override;
	
	virtual void BeginPlay() override;
	virtual void OnEnableStateChanged(const bool bIsEnabled) override;
};
