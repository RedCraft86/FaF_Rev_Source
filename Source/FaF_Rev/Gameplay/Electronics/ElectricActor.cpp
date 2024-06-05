// Copyright (C) RedCraft86. All Rights Reserved.

#include "ElectricActor.h"

AElectricActorBase::AElectricActorBase()
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
	OnEnergyChanged(GetEnergy());
}

void AElectricActorBase::RemoveEnergy(const FName Key)
{
	CachedEnergy.Key = false;
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

void AElectricActorBase::OnEnergyChanged(const uint8 Total)
{
	EnergyChangedEvent(Total);
}
