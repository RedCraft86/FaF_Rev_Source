// Copyright (C) RedCraft86. All Rights Reserved.

#include "ElectricLight.h"
#include "Components/LightComponent.h"

AElectricLightBase::AElectricLightBase()
	: bFlicker(false), FlickerRate(0.4f), LightFlickerRange({0.1f, 1.5f}), MeshFlickerRange({-0.1f, 1.0f})
	, FlickerTime(0.0f), FlickerRange(FVector2D::UnitY()), FlickerTimeRange(FVector2D::UnitY())
{
	SmartCulling = CreateDefaultSubobject<USmartCullingComponent>("SmartCulling");
	MinEnergy = 0;

	FlickerCurve.GetRichCurve()->UpdateOrAddKey(0.0f, 1.0f);
	FlickerCurve.GetRichCurve()->UpdateOrAddKey(0.0f, 1.0f);
	FlickerCurve.GetRichCurve()->UpdateOrAddKey(0.06f, 0.28f);
	FlickerCurve.GetRichCurve()->UpdateOrAddKey(0.1f, 0.95f);
	FlickerCurve.GetRichCurve()->UpdateOrAddKey(0.13f, 0.15f);
	FlickerCurve.GetRichCurve()->UpdateOrAddKey(0.17f, 0.69f);
	FlickerCurve.GetRichCurve()->UpdateOrAddKey(0.2f, 0.37f);
	FlickerCurve.GetRichCurve()->UpdateOrAddKey(0.23f, 1.24f);
	FlickerCurve.GetRichCurve()->UpdateOrAddKey(0.26f, 0.69f);
	FlickerCurve.GetRichCurve()->UpdateOrAddKey(0.3f, 1.33f);
	FlickerCurve.GetRichCurve()->UpdateOrAddKey(0.33f, 0.32f);
	FlickerCurve.GetRichCurve()->UpdateOrAddKey(0.36f, 0.88f);
	FlickerCurve.GetRichCurve()->UpdateOrAddKey(0.4f, 0.65f);
	FlickerCurve.GetRichCurve()->UpdateOrAddKey(0.42f, 0.97f);
	FlickerCurve.GetRichCurve()->UpdateOrAddKey(0.44f, 0.43f);
	FlickerCurve.GetRichCurve()->UpdateOrAddKey(0.46f, 0.69f);
	FlickerCurve.GetRichCurve()->UpdateOrAddKey(0.48f, 0.58f);
	FlickerCurve.GetRichCurve()->UpdateOrAddKey(0.5f, 1.0f);
}

void AElectricLightBase::SetFlickerState(const bool bNewFlicker)
{
	if (bFlicker != bNewFlicker)
	{
		bFlicker = bNewFlicker;
		OnFlickerChanged(bFlicker);
	}
}

void AElectricLightBase::OnFlickerUpdate(const float DeltaTime)
{
	const float Value = FMath::Max(0.0f, FlickerCurve.GetValue(FlickerTime));
	for (ULightComponent* Light : CachedLights)
	{
		if (Light) Light->SetIntensity(FMath::Max(0.0f,FMath::GetMappedRangeValueClamped(
			FlickerRange, LightFlickerRange, Value) * CachedIntensity.FindRef(Light)));
	}
	for (const TPair<UStaticMeshComponent*, bool>& Mesh : CachedMeshes)
	{
		if (Mesh.Key)
			Mesh.Key->SetCustomPrimitiveDataFloat(6, FMath::Max(0.0f, FMath::GetMappedRangeValueClamped(
				FlickerRange, MeshFlickerRange, Value)));
	}
	
	FlickerTime += DeltaTime * FlickerRate;
	if (FlickerTime > FlickerTimeRange.Y)
	{
		FlickerTime = FlickerTimeRange.X;
	}
}

void AElectricLightBase::OnFlickerChanged(const bool bState)
{
	FlickerCurve.GetValueRange(FlickerRange.X, FlickerRange.Y);
	FlickerCurve.GetTimeRange(FlickerTimeRange.X, FlickerTimeRange.Y);
	FlickerTime = FlickerTimeRange.X;
		
	SetActorTickEnabled(PrimaryActorTick.bStartWithTickEnabled || bFlicker);
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
		Mesh.Key->SetDefaultCustomPrimitiveDataFloat(6, 1.0f);
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
	if (!IsHidden() && bFlicker && bCachedState)
		OnFlickerUpdate(DeltaSeconds);
}

void AElectricLightBase::BeginPlay()
{
	Super::BeginPlay();
	CachedLights.Empty();
	CachedMeshes.Empty();
	CachedIntensity.Empty();
	GetLightInfo(CachedLights, CachedMeshes);
	CachedIntensity.Reserve(CachedLights.Num());
	for (ULightComponent* Light : CachedLights)
	{
		if (Light) CachedIntensity.Add(Light, Light->Intensity);
	}

	bCachedState = bPreviewState;
	OnFlickerChanged(bFlicker);
}

#if WITH_EDITORONLY_DATA
void AElectricLightBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (!FApp::IsGame())
	{
		CachedLights.Empty();
		CachedMeshes.Empty();
		CachedIntensity.Empty();
		GetLightInfo(CachedLights, CachedMeshes);
		CachedIntensity.Reserve(CachedLights.Num());
		for (ULightComponent* Light : CachedLights)
		{
			if (Light) CachedIntensity.Add(Light, Light->Intensity);
		}
		
		bCachedState = bPreviewState;
		OnStateChanged(bPreviewState);
		OnFlickerChanged(bFlicker);
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
