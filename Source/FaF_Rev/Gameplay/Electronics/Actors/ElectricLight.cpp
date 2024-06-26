// Copyright (C) RedCraft86. All Rights Reserved.

#include "ElectricLight.h"
#include "Components/LightComponent.h"

AElectricLightBase::AElectricLightBase() : bFlicker(false), FlickerTime(0.0f)
	, FlickerTimeRange(FVector2D::UnitY()), FlickerRange(FVector2D::UnitY())
{
	MinEnergy = 0;
	SmartCulling = CreateDefaultSubobject<USmartCullingComponent>("SmartCulling");
}

void AElectricLightBase::SetFlickerState(const bool bNewFlicker)
{
	if (bFlicker != bNewFlicker)
	{
		bFlicker = bNewFlicker;
		FlickerCurve.GetValueRange(FlickerRange.X, FlickerRange.Y);
		FlickerCurve.GetTimeRange(FlickerTimeRange.X, FlickerTimeRange.Y);
		FlickerTime = FlickerTimeRange.X;
		
		SetActorTickEnabled(PrimaryActorTick.bStartWithTickEnabled || bFlicker);
	}
}

void AElectricLightBase::OnFlickerUpdate(const float DeltaTime)
{
	const float Value = FMath::Max(0.0f, FlickerCurve.GetValue(FlickerTime));
	for (ULightComponent* Light : CachedLights)
	{
		if (Light) Light->SetIntensity(CachedIntensity.FindRef(Light) * Value);
	}
	for (const TPair<UStaticMeshComponent*, bool>& Mesh : CachedMeshes)
	{
		if (Mesh.Key) Mesh.Key->SetCustomPrimitiveDataFloat(6, Value);
	}
	
	FlickerTime += DeltaTime;
	if (FlickerTime > FlickerTimeRange.Y)
	{
		FlickerTime = FlickerTimeRange.X;
	}
}

void AElectricLightBase::OnStateChanged(const bool bState)
{
	for (ULightComponent* Light : CachedLights)
	{
		if (Light) Light->SetVisibility(bState);
	}
	for (const TPair<UStaticMeshComponent*, bool>& Mesh : CachedMeshes)
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

void AElectricLightBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bFlicker && bCachedState)
		OnFlickerUpdate(DeltaSeconds);
}

void AElectricLightBase::BeginPlay()
{
	Super::BeginPlay();
	GetLightInfo(CachedLights, CachedMeshes);
	for (ULightComponent* Light : CachedLights)
	{
		if (Light) CachedIntensity.Add(Light, Light->Intensity);
	}
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
