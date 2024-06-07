// Copyright (C) RedCraft86. All Rights Reserved.

#include "ElectricActor.h"

AElectricActorBase::AElectricActorBase() : MinEnergy(1), bCachedState(false)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
	
#if WITH_EDITORONLY_DATA
	SceneRoot->bVisualizeComponent = true;
#endif
}

void AElectricActorBase::AddEnergy(const FName Key, const uint8 Value)
{
	CachedEnergy.Key = false;
	Energy.Add(Key, Value);
	OnEnergyChanged(GetEnergy());
}

void AElectricActorBase::RemoveEnergy(const FName Key)
{
	CachedEnergy.Key = false;
	Energy.Remove(Key);
	OnEnergyChanged(GetEnergy());
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
	return GetEnergy() >= MinEnergy;
}

void AElectricActorBase::OnEnergyChanged(const uint8 Total)
{
	const bool bState = Total >= MinEnergy;
	if (bCachedState != bState)
	{
		bCachedState = bState;
		OnStateChanged(bState);
	}
	
	EnergyChangedEvent(Total);
}

void AElectricActorBase::OnStateChanged(const bool bState)
{
	StateChangedEvent(bState);
}

void AElectricActorBase::BeginPlay()
{
	Super::BeginPlay();
	OnStateChanged(GetState());
}
