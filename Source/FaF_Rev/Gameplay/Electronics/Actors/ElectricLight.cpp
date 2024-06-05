// Copyright (C) RedCraft86. All Rights Reserved.

#include "ElectricLight.h"
#include "Components/LightComponent.h"

void AElectricLightBase::SetFlickerState(const bool bNewFlicker)
{
	if (bFlicker != bNewFlicker)
	{
		bFlicker = bNewFlicker;
		OnFlickerUpdate();
	}
}

void AElectricLightBase::OnFlickerUpdate() const
{
	TSet<ULightComponent*> Lights;
	TArray<FLightMeshData> Meshes;
	GetLightInfo(Lights, Meshes);

	for (ULightComponent* Light : Lights)
	{
		if (Light) Light->SetLightFunctionMaterial(bFlicker ? FlickerFunction : nullptr);
	}
	
	for (const FLightMeshData& Mesh : Meshes)
	{
		if (!Mesh.MeshComponent) continue;
		Mesh.MeshComponent->SetCustomPrimitiveDataFloat(Mesh.FlickerIndex, bFlicker ? 1.0f : 0.0f);
	}
}

void AElectricLightBase::OnStateChanged(const bool bState)
{
	OnFlickerUpdate();
	
	TSet<ULightComponent*> Lights;
	TArray<FLightMeshData> Meshes;
	GetLightInfo(Lights, Meshes);
	
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
			for (const TPair<uint8, FVector2D>& Value : Mesh.PrimitiveValues)
			{
				Mesh.MeshComponent->SetCustomPrimitiveDataFloat(Value.Key, bState ? Value.Value.Y : Value.Value.X);
			}
		}
	}
	
	Super::OnStateChanged(bState);
}

#if WITH_EDITORONLY_DATA
void AElectricLightBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	OnStateChanged(bPreviewState);
	bCachedState = bPreviewState;
}
#endif
