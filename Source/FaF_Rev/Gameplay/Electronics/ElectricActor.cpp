// Copyright (C) RedCraft86. All Rights Reserved.

#include "ElectricActor.h"

AElectricActorBase::AElectricActorBase() : MinEnergy(1), bCachedState(false)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
#if WITH_EDITORONLY_DATA
	SceneRoot->bVisualizeComponent = true;
#endif
}

void AElectricActorBase::AddEnergy(const FName Key, const uint8 Value)
{
	if (Value == 0) RemoveEnergy(Key);
	CachedEnergy.Key = false;
	Energy.Add(Key, Value);
	OnEnergyChanged();
}

void AElectricActorBase::RemoveEnergy(const FName Key)
{
	if (!Energy.Contains(Key)) return;
	CachedEnergy.Key = false;
	Energy.Remove(Key);
	OnEnergyChanged();
}

uint8 AElectricActorBase::GetEnergy()
{
	if (CachedEnergy.Key)
		return CachedEnergy.Value;
	
	uint8 Out = 0;
	for (const TPair<FName, uint8>& Pair : Energy)
	{
		if (Out >= 255) break;
		Out += Pair.Value;
	}

	CachedEnergy.Key = true;
	CachedEnergy.Value = Out;
	return Out;
}

bool AElectricActorBase::GetState()
{
	return IsEnabled() && GetEnergy() >= MinEnergy;
}

void AElectricActorBase::OnEnergyChanged()
{
	const uint8 TotalEnergy = GetEnergy();
	const bool bState = IsEnabled() && TotalEnergy >= MinEnergy;
	if (bCachedState != bState)
	{
		bCachedState = bState;
		OnStateChanged(bCachedState);
	}
	
	EnergyChangedEvent(TotalEnergy);
}

void AElectricActorBase::OnStateChanged(const bool bInState)
{
	StateChangedEvent(bInState);
}

void AElectricActorBase::BeginPlay()
{
	Super::BeginPlay();
	OnStateChanged(GetState());
}

void AElectricActorBase::OnEnableStateChanged(const bool bIsEnabled)
{
	OnStateChanged(GetState());
}
