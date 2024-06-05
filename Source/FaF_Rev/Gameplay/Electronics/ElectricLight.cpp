// Copyright (C) RedCraft86. All Rights Reserved.

#include "ElectricLight.h"
#include "Components/LightComponent.h"

bool AElectricLightBase::GetState()
{
	return GetEnergy() >= MinEnergy;
}

void AElectricLightBase::SetBrokenState(const bool bNewBroken)
{
	if (bBroken != bNewBroken)
	{
		bBroken = bNewBroken;
		OnLightUpdate(bCachedState);
	}
}

void AElectricLightBase::OnLightUpdate(const bool bState)
{
	TSet<ULightComponent*> Lights;
	TArray<FLightMeshData> Meshes;
	GetLightInfo(Lights, Meshes);

	for (ULightComponent* Light : Lights)
	{
		if (Light) Light->SetLightFunctionMaterial(bBroken ? FlickerFunction : nullptr);
	}
	
	for (const FLightMeshData& Mesh : Meshes)
	{
		if (!Mesh.MeshComponent) continue;
		Mesh.MeshComponent->SetCustomPrimitiveDataFloat(Mesh.FlickerIndex, bBroken ? 1.0f : 0.0f);
	}

	if (bCachedState == bState) return;
	bCachedState = bState;

	for (ULightComponent* Light : Lights)
	{
		if (Light) Light->SetHiddenInGame(!bState);
	}
	
	for (const FLightMeshData& Mesh : Meshes)
	{
		if (!Mesh.MeshComponent) continue;
		if (Mesh.bHideCompletely)
		{
			Mesh.MeshComponent->SetHiddenInGame(!bState);
		}
		else
		{
			Mesh.MeshComponent->SetHiddenInGame(true);
			for (const TPair<uint8, FIntPoint>& Value : Mesh.PrimitiveValues)
			{
				Mesh.MeshComponent->SetCustomPrimitiveDataFloat(Value.Key, bState ? Value.Value.Y : Value.Value.X);
			}
		}
	}
	
	StateChangedEvent(bState);
}

void AElectricLightBase::OnEnergyChanged(const uint8 Total)
{
	OnLightUpdate(Total >= MinEnergy);
	Super::OnEnergyChanged(Total);
}

void AElectricLightBase::BeginPlay()
{
	Super::BeginPlay();
	OnLightUpdate(false);
}

#if WITH_EDITORONLY_DATA
void AElectricLightBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	OnLightUpdate(bPreviewState);
}
#endif
