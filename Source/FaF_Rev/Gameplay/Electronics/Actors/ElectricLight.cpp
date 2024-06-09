// Copyright (C) RedCraft86. All Rights Reserved.

#include "ElectricLight.h"
#include "Components/LightComponent.h"

AElectricLightBase::AElectricLightBase() : bFlicker(false)
{
	MinEnergy = 0;
	SmartCulling = CreateDefaultSubobject<USmartCullingComponent>("SmartCulling");
}

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
	TMap<UStaticMeshComponent*, bool> Meshes;
	GetLightInfo(Lights, Meshes);

	for (ULightComponent* Light : Lights)
	{
		if (Light) Light->SetLightFunctionMaterial(bFlicker ? FlickerFunction : nullptr);
	}
	
	for (const TPair<UStaticMeshComponent*, bool>& Mesh : Meshes)
	{
		if (Mesh.Key) Mesh.Key->SetDefaultCustomPrimitiveDataFloat(6, bFlicker ? MeshProperties.Flicker : 0.0f);
	}
}

void AElectricLightBase::OnStateChanged(const bool bState)
{
	OnFlickerUpdate();
	
	TSet<ULightComponent*> Lights;
	TMap<UStaticMeshComponent*, bool> Meshes;
	GetLightInfo(Lights, Meshes);
	
	for (ULightComponent* Light : Lights)
	{
		if (Light) Light->SetVisibility(bState);
	}
	
	for (const TPair<UStaticMeshComponent*, bool>& Mesh : Meshes)
	{
		if (!Mesh.Key) continue;
		if (Mesh.Value)
		{
			Mesh.Key->SetVisibility(bState);
		}
		else
		{
			Mesh.Key->SetVisibility(true);
			if (Mesh.Key) Mesh.Key->SetDefaultCustomPrimitiveDataFloat(5, bState ? 1.0f : 0.0f);
		}
	}
	
	Super::OnStateChanged(bState);
}

#if WITH_EDITORONLY_DATA
void AElectricLightBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (!FApp::IsGame())
	{
		bCachedState = bPreviewState;
		OnStateChanged(bPreviewState);
	}
}
#endif
void AElectricLightBase::SetLightMeshSettings(UStaticMeshComponent* Target, const ULightComponent* Source, const FLightMeshProperties& Properties)
{
	if (!Target || !Source) return;

	FLinearColor Color = Source->GetLightColor();
	if (Source->bUseTemperature)
	{
		Color *= FLinearColor::MakeFromColorTemperature(Source->Temperature);
	}

	Color.A = Source->Intensity * FMath::Max(0.0f, Properties.Intensity);

	Target->SetDefaultCustomPrimitiveDataVector4(0, Color);
	Target->SetDefaultCustomPrimitiveDataFloat(4, Properties.Fresnel);
}
