// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once 

#include "RCLightingTypes.generated.h"

USTRUCT(BlueprintType, DisplayName = "Light Draw Distance")
struct RCRUNTIME_API FRCLightDrawDistance
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightDrawDistance")
        bool bUseDrawDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightDrawDistance", meta = (EditCondition = "bUseDrawDistance", ClampMin = 0.0f, UIMin = 0.0f))
        float MaxDrawDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightDrawDistance", meta = (EditCondition = "bUseDrawDistance", ClampMin = 0.0f, UIMin = 0.0f))
        float MaxDistanceFadeRange;

    friend FArchive& operator<<(FArchive& Ar, FRCLightDrawDistance& DrawDistance)
    {
        Ar << DrawDistance.bUseDrawDistance;
        Ar << DrawDistance.MaxDrawDistance;
        Ar << DrawDistance.MaxDistanceFadeRange;
        return Ar;
    }

    FRCLightDrawDistance()
        : bUseDrawDistance(true)
        , MaxDrawDistance(5000.0f)
        , MaxDistanceFadeRange(1000.0f)
    {}

    float GetMaxDrawDistance() const
    {
        return bUseDrawDistance ? FMath::Max(0.0f, MaxDrawDistance) : 0.0f;
    }

    float GetMaxDistanceFadeRange() const
    {
        return bUseDrawDistance ? FMath::Max(0.0f, MaxDistanceFadeRange) : 0.0f;
    }

    void IdentifyDrawDistanceUsage()
    {
        bUseDrawDistance = (MaxDrawDistance + MaxDistanceFadeRange) > 0.0f;
    }
};

USTRUCT(BlueprintType, DisplayName = "Base Light Properties")
struct RCRUNTIME_API FRCBaseLightProperties
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (DisplayPriority = 0))
        ELightUnits IntensityUnits;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (ClampMin = 0.0f, UIMin = 0.0f, ShouldShowInViewport = true, DisplayPriority = 0))
        float Intensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (HideAlphaChannel, ShouldShowInViewport = true, DisplayPriority = 0))
        FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (ClampMin = 0.0f, UIMin = 8.0f, UIMax = 16384.0f, ShouldShowInViewport = true, DisplayPriority = 0))
        float AttenuationRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (InlineEditConditionToggle, DisplayPriority = 2))
        bool bUseTemperature;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (UIMin = 1700.0f, UIMax = 12000.0f, ShouldShowInViewport = true, EditCondition = "bUseTemperature", DisplayPriority = 2))
        float Temperature;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (UIMin = 0.0f, UIMax = 6.0f, DisplayPriority = 2))
        float IndirectIntensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (UIMin = 0.25f, UIMax = 4.0f, DisplayPriority = 2))
        float VolumetricScatteringIntensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (DisplayPriority = 2))
        bool bLightCastShadows;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (DisplayPriority = 2))
        bool bCastVolumetricShadows;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f, DisplayPriority = 3))
        float SpecularScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (DisplayPriority = 3))
        FRCLightDrawDistance DrawDistance;
    
    friend FArchive& operator<<(FArchive& Ar, FRCBaseLightProperties& Properties)
    {
        Ar << Properties.IntensityUnits;
        Ar << Properties.Intensity;
        Ar << Properties.Color;
        Ar << Properties.AttenuationRadius;
        Ar << Properties.bUseTemperature;
        Ar << Properties.Temperature;
        Ar << Properties.IndirectIntensity;
        Ar << Properties.VolumetricScatteringIntensity;
        Ar << Properties.bLightCastShadows;
        Ar << Properties.bCastVolumetricShadows;
        Ar << Properties.SpecularScale;
        Ar << Properties.DrawDistance;
        return Ar;
    }

    FRCBaseLightProperties()
        : IntensityUnits(ELightUnits::Candelas)
        , Intensity(8.0f)
        , Color(FLinearColor::White)
        , AttenuationRadius(1000.0f)
        , bUseTemperature(false)
        , Temperature(6500.0f)
        , IndirectIntensity(1.0f)
        , VolumetricScatteringIntensity(1.0f)
        , bLightCastShadows(true)
        , bCastVolumetricShadows(false)
        , SpecularScale(1.0f)
        , DrawDistance({})
    {}

    FRCBaseLightProperties GetBaseLightProperties() const
    {
        FRCBaseLightProperties Properties;
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

    void CopyFrom(const FRCBaseLightProperties& Properties)
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

    void CopyBasicLightData(const FRCBaseLightProperties& Properties)
    {
        IntensityUnits = Properties.IntensityUnits;
        Intensity = Properties.Intensity;
        Color = Properties.Color;
        bUseTemperature = Properties.bUseTemperature;
        Temperature = Properties.Temperature;
    }
};

USTRUCT(BlueprintType, DisplayName = "Point Light Properties")
struct RCRUNTIME_API FRCPointLightProperties : public FRCBaseLightProperties
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (ShouldShowInViewport = true, DisplayPriority = 1, ClampMin = 0.0f, UIMin = 0.0f))
        float SourceRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (DisplayPriority = 1, ClampMin = 0.0f, UIMin = 0.0f))
        float SoftSourceRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (DisplayPriority = 1, ClampMin = 0.0f, UIMin = 0.0f))
        float SourceLength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (InlineEditConditionToggle, DisplayPriority = 3))
        bool bUseInverseSquaredFalloff;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (UIMin = 2.0f, UIMax = 16.0f, EditCondition = "!bUseInverseSquaredFalloff", DisplayPriority = 3))
        float LightFalloffExponent;

    friend FArchive& operator<<(FArchive& Ar, FRCPointLightProperties& Properties)
    {
        Ar << Properties.SourceRadius;
        Ar << Properties.SoftSourceRadius;
        Ar << Properties.SourceLength;
        Ar << Properties.bUseInverseSquaredFalloff;
        Ar << Properties.LightFalloffExponent;
        return Ar;
    }

    FRCPointLightProperties()
        : SourceRadius(0.0f)
        , SoftSourceRadius(0.0f)
        , SourceLength(0.0f)
        , bUseInverseSquaredFalloff(false)
        , LightFalloffExponent(8.0f)
    {}
};

USTRUCT(BlueprintType, DisplayName = "Spot Light Properties")
struct RCRUNTIME_API FRCSpotLightProperties : public FRCBaseLightProperties
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (UIMin = 1.0f, UIMax = 80.0f, ShouldShowInViewport = true, DisplayPriority = 1))
        float InnerConeAngle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (UIMin = 1.0f, UIMax = 80.0f, ShouldShowInViewport = true, DisplayPriority = 1))
        float OuterConeAngle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (ShouldShowInViewport = true, DisplayPriority = 1, ClampMin = 0.0f, UIMin = 0.0f))
        float SourceRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (DisplayPriority = 1, ClampMin = 0.0f, UIMin = 0.0f))
        float SoftSourceRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (DisplayPriority = 1, ClampMin = 0.0f, UIMin = 0.0f))
        float SourceLength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (InlineEditConditionToggle, DisplayPriority = 3))
        bool bUseInverseSquaredFalloff;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (UIMin = 2.0f, UIMax = 16.0f, EditCondition = "!bUseInverseSquaredFalloff", DisplayPriority = 3))
        float LightFalloffExponent;

    friend FArchive& operator<<(FArchive& Ar, FRCSpotLightProperties& Properties)
    {
        Ar << Properties.InnerConeAngle;
        Ar << Properties.OuterConeAngle;
        Ar << Properties.SourceLength;
        Ar << Properties.bUseInverseSquaredFalloff;
        Ar << Properties.LightFalloffExponent;
        return Ar;
    }

    FRCSpotLightProperties()
        : InnerConeAngle(0.0f)
        , OuterConeAngle(44.0f)
        , SourceRadius(0.0f)
        , SoftSourceRadius(0.0f)
        , SourceLength(0.0f)
        , bUseInverseSquaredFalloff(false)
        , LightFalloffExponent(8.0f)
    {}
};

USTRUCT(BlueprintType, DisplayName = "Rect Light Properties")
struct RCRUNTIME_API FRCRectLightProperties : public FRCBaseLightProperties
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (DisplayPriority = 1, ClampMin = 10.0f, UIMin = 10.0f))
        float SourceWidth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (DisplayPriority = 1, ClampMin = 10.0f, UIMin = 10.0f))
        float SourceHeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (ClampMin = 0.0f, ClampMax = 90.0f, UIMin = 0.0f, UIMax = 90.0f, DisplayPriority = 1))
        float BarnDoorAngle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (ClampMin = 1.0f, UIMin = 1.0f, DisplayPriority = 1))
        float BarnDoorLength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (DisplayPriority = 1))
        TObjectPtr<UTexture> SourceTexture;

    friend FArchive& operator<<(FArchive& Ar, FRCRectLightProperties& Properties)
    {
        Ar << Properties.SourceWidth;
        Ar << Properties.SourceHeight;
        Ar << Properties.BarnDoorAngle;
        Ar << Properties.BarnDoorLength;
        Ar << Properties.SourceTexture;
        return Ar;
    }

    FRCRectLightProperties()
        : SourceWidth(64.0f)
        , SourceHeight(64.0f)
        , BarnDoorAngle(88.0f)
        , BarnDoorLength(20.0f)
        , SourceTexture(nullptr)
    {}
};