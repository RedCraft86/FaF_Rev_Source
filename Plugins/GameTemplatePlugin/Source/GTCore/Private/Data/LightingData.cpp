// Copyright (C) RedCraft86. All Rights Reserved.

#include "Data/LightingData.h"
#include "Components/LightComponent.h"
#include "Components/LocalLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/RectLightComponent.h"
#include "Components/SpotLightComponent.h"


FBaseLightProperties FBaseLightProperties::GetBaseLightProperties() const
{
	FBaseLightProperties Properties;
	Properties.IntensityUnits = IntensityUnits;
	Properties.Intensity = Intensity;
	Properties.Color = Color;
	Properties.AttenuationRadius = AttenuationRadius;
	Properties.bUseTemperature = bUseTemperature;
	Properties.Temperature = Temperature;
	Properties.IndirectIntensity = IndirectIntensity;
	Properties.VolumetricScatteringIntensity = VolumetricScatteringIntensity;
	Properties.bLightCastShadows = bLightCastShadows;
	Properties.bCastVolumetricShadows = bCastVolumetricShadows;
	Properties.SpecularScale = SpecularScale;
	Properties.DrawDistance = DrawDistance;

	return Properties;
}

void FBaseLightProperties::CopyFrom(const FBaseLightProperties& Properties)
{
	IntensityUnits = Properties.IntensityUnits;
	Intensity = Properties.Intensity;
	Color = Properties.Color;
	AttenuationRadius = Properties.AttenuationRadius;
	bUseTemperature = Properties.bUseTemperature;
	Temperature = Properties.Temperature;
	IndirectIntensity = Properties.IndirectIntensity;
	VolumetricScatteringIntensity = Properties.VolumetricScatteringIntensity;
	bLightCastShadows = Properties.bLightCastShadows;
	bCastVolumetricShadows = Properties.bCastVolumetricShadows;
	SpecularScale = Properties.SpecularScale;
	DrawDistance = Properties.DrawDistance;
}

void FBaseLightProperties::CopyBasicData(const FBaseLightProperties& Properties)
{
	IntensityUnits = Properties.IntensityUnits;
	Intensity = Properties.Intensity;
	Color = Properties.Color;
	bUseTemperature = Properties.bUseTemperature;
	Temperature = Properties.Temperature;
}

FBaseLightProperties ULightingDataLibrary::GetBaseProperties_Point(const FPointLightProperties& Target)
{
	return Target.GetBaseLightProperties();
}

FBaseLightProperties ULightingDataLibrary::GetBaseProperties_Spot(const FSpotLightProperties& Target)
{
	return Target.GetBaseLightProperties();
}

FBaseLightProperties ULightingDataLibrary::GetBaseProperties_Rect(const FRectLightProperties& Target)
{
	return Target.GetBaseLightProperties();
}

FPointLightProperties ULightingDataLibrary::GetPointProperties(const FBaseLightProperties& Target, const bool bOnlyBasicData)
{
	FPointLightProperties Result;
	if (bOnlyBasicData) Result.CopyBasicData(Target); else Result.CopyFrom(Target);
	return Result;
}

FSpotLightProperties ULightingDataLibrary::GetSpotProperties(const FBaseLightProperties& Target, const bool bOnlyBasicData)
{
	FSpotLightProperties Result;
	if (bOnlyBasicData) Result.CopyBasicData(Target); else Result.CopyFrom(Target);
	return Result;
}

FRectLightProperties ULightingDataLibrary::GetRectProperties(const FBaseLightProperties& Target, const bool bOnlyBasicData)
{
	FRectLightProperties Result;
	if (bOnlyBasicData) Result.CopyBasicData(Target); else Result.CopyFrom(Target);
	return Result;
}

void ULightingDataLibrary::SetLightDrawDistanceSettings(ULightComponent* Target, const FLightDrawDistance& Settings)
{
	if (IsValid(Target))
	{
		Target->MaxDrawDistance = Settings.GetMaxDrawDistance();
		Target->MaxDistanceFadeRange = Settings.GetMaxDistanceFadeRange();
	}
}

FLightDrawDistance ULightingDataLibrary::GetLightDrawDistanceSettings(const ULightComponent* Target)
{
	FLightDrawDistance Result;
	if (IsValid(Target))
	{
		Result.MaxDrawDistance = Target->MaxDrawDistance;
		Result.MaxDistanceFadeRange = Target->MaxDistanceFadeRange;
		Result.bUseDrawDistance = Result.IsValidData();
	}

	return Result;
}

void ULightingDataLibrary::SetBaseLightProperties(ULightComponent* Target, const FBaseLightProperties& Properties)
{
	if (IsValid(Target))
	{
		Target->SetIntensity(Properties.Intensity);
		Target->SetLightColor(Properties.Color);
		Target->SetUseTemperature(Properties.bUseTemperature);
		Target->SetTemperature(Properties.Temperature);
		Target->SetIndirectLightingIntensity(Properties.IndirectIntensity);
		Target->SetVolumetricScatteringIntensity(Properties.VolumetricScatteringIntensity);
		Target->SetCastShadows(Properties.bLightCastShadows);
		Target->SetCastVolumetricShadow(Properties.bCastVolumetricShadows);
		Target->SetSpecularScale(Properties.SpecularScale);

		// NOTE: Intensity Units and Attenuation Radius are not part of the ULightComponent so we need to cast to ULocalLightComponent
		if (ULocalLightComponent* LocalLight = Cast<ULocalLightComponent>(Target))
		{
			LocalLight->SetIntensityUnits(Properties.IntensityUnits);
			LocalLight->SetAttenuationRadius(Properties.AttenuationRadius);
		}

		SetLightDrawDistanceSettings(Target, Properties.DrawDistance);
	}
}

FBaseLightProperties ULightingDataLibrary::GetBaseLightProperties(const ULightComponent* Target)
{
	FBaseLightProperties OutData;
	if (Target)
	{
		OutData.Intensity = Target->Intensity;
		OutData.Color = Target->LightColor;
		OutData.bUseTemperature = Target->bUseTemperature;
		OutData.Temperature = Target->Temperature;
		OutData.IndirectIntensity = Target->IndirectLightingIntensity;
		OutData.VolumetricScatteringIntensity = Target->VolumetricScatteringIntensity;
		OutData.bLightCastShadows = Target->CastShadows;
		OutData.bCastVolumetricShadows = Target->bCastVolumetricShadow;
		OutData.SpecularScale = Target->SpecularScale;

		// NOTE: Intensity Units and Attenuation Radius are not part of the ULightComponent so we need to cast to ULocalLightComponent
		if (const ULocalLightComponent* LocalLight = Cast<ULocalLightComponent>(Target))
		{
			OutData.IntensityUnits = LocalLight->IntensityUnits;
			OutData.AttenuationRadius = LocalLight->AttenuationRadius;
		}

		OutData.DrawDistance = GetLightDrawDistanceSettings(Target);
	}
	
	return OutData;
}

void ULightingDataLibrary::SetPointLightProperties(UPointLightComponent* Target, const FPointLightProperties& Properties)
{
	if (IsValid(Target))
	{
		SetBaseLightProperties(Target, Properties);
		Target->SetSourceRadius(Properties.SourceRadius);
		Target->SetSoftSourceRadius(Properties.SoftSourceRadius);
		Target->SetSourceLength(Properties.SourceLength);
		Target->bUseInverseSquaredFalloff = Properties.bUseInverseSquaredFalloff;
		Target->SetLightFalloffExponent(Properties.LightFalloffExponent);
	}
}

FPointLightProperties ULightingDataLibrary::GetPointLightProperties(const UPointLightComponent* Target)
{
	FPointLightProperties OutData;
	if (Target)
	{
		OutData.CopyFrom(GetBaseLightProperties(Target));
		OutData.SourceRadius = Target->SourceRadius;
		OutData.SoftSourceRadius = Target->SoftSourceRadius;
		OutData.SourceLength = Target->SourceLength;
		OutData.bUseInverseSquaredFalloff = Target->bUseInverseSquaredFalloff;
		OutData.LightFalloffExponent = Target->LightFalloffExponent;
	}
	
	return OutData;
}

void ULightingDataLibrary::SetSpotLightProperties(USpotLightComponent* Target, const FSpotLightProperties& Properties)
{
	if (IsValid(Target))
	{
		SetBaseLightProperties(Target, Properties);
		Target->SetInnerConeAngle(Properties.InnerConeAngle);
		Target->SetOuterConeAngle(Properties.OuterConeAngle);
		Target->SetSourceRadius(Properties.SourceRadius);
		Target->SetSoftSourceRadius(Properties.SoftSourceRadius);
		Target->SetSourceLength(Properties.SourceLength);
		Target->bUseInverseSquaredFalloff = Properties.bUseInverseSquaredFalloff;
		Target->SetLightFalloffExponent(Properties.LightFalloffExponent);
	}
}

FSpotLightProperties ULightingDataLibrary::GetSpotLightProperties(const USpotLightComponent* Target)
{
	FSpotLightProperties OutData;
	if (Target)
	{
		OutData.CopyFrom(GetBaseLightProperties(Target));
		OutData.InnerConeAngle = Target->InnerConeAngle;
		OutData.OuterConeAngle = Target->OuterConeAngle;
		OutData.SourceRadius = Target->SourceRadius;
		OutData.SoftSourceRadius = Target->SoftSourceRadius;
		OutData.SourceLength = Target->SourceLength;
		OutData.bUseInverseSquaredFalloff = Target->bUseInverseSquaredFalloff;
		OutData.LightFalloffExponent = Target->LightFalloffExponent;
	}

	return OutData;
}

void ULightingDataLibrary::SetRectLightProperties(URectLightComponent* Target, const FRectLightProperties& Properties)
{
	if (IsValid(Target))
	{
		SetBaseLightProperties(Target, Properties);
		Target->SetSourceWidth(Properties.SourceWidth);
		Target->SetSourceHeight(Properties.SourceHeight);
		Target->SetBarnDoorAngle(Properties.BarnDoorAngle);
		Target->SetBarnDoorLength(Properties.BarnDoorLength);
		Target->SetSourceTexture(Properties.SourceTexture);
	}
}

FRectLightProperties ULightingDataLibrary::GetRectLightProperties(const URectLightComponent* Target)
{
	FRectLightProperties OutData;
	if (Target)
	{
		OutData.CopyFrom(GetBaseLightProperties(Target));
		OutData.SourceWidth = Target->SourceWidth;
		OutData.SourceHeight = Target->SourceHeight;
		OutData.BarnDoorAngle = Target->BarnDoorAngle;
		OutData.BarnDoorLength = Target->BarnDoorLength;
		OutData.SourceTexture = Target->SourceTexture;
	}
	
	return OutData;
}
