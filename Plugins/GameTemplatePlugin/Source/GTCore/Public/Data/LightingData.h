// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "LightingData.generated.h"

class ULightComponent;
class UPointLightComponent;
class USpotLightComponent;
class URectLightComponent;

USTRUCT(BlueprintType)
struct GTCORE_API FLightDrawDistance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties")
		bool bUseDrawDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (EditCondition = "bUseDrawDistance", ClampMin = 0.0f, UIMin = 0.0f))
		float MaxDrawDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightProperties", meta = (EditCondition = "bUseDrawDistance", ClampMin = 0.0f, UIMin = 0.0f))
		float MaxDistanceFadeRange;

    FLightDrawDistance()
        : bUseDrawDistance(true)
        , MaxDrawDistance(5000.0f)
        , MaxDistanceFadeRange(1000.0f)
    {}
    
	friend FArchive& operator<<(FArchive& Ar, FLightDrawDistance& DrawDistance)
	{
		Ar << DrawDistance.bUseDrawDistance;
		Ar << DrawDistance.MaxDrawDistance;
		Ar << DrawDistance.MaxDistanceFadeRange;
		return Ar;
	}

	FORCEINLINE float GetMaxDrawDistance() const
	{
		return bUseDrawDistance ? FMath::Max(0.0f, MaxDrawDistance) : 0.0f;
	}

	FORCEINLINE float GetMaxDistanceFadeRange() const
	{
		return bUseDrawDistance ? FMath::Max(0.0f, MaxDistanceFadeRange) : 0.0f;
	}

    FORCEINLINE bool IsValidData() const
    {
	    return MaxDistanceFadeRange < MaxDrawDistance && MaxDistanceFadeRange + MaxDrawDistance > 0.0f;
	}
};

USTRUCT(BlueprintType)
struct GTCORE_API FBaseLightProperties
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
        FLightDrawDistance DrawDistance;

    FBaseLightProperties()
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
    
    friend FArchive& operator<<(FArchive& Ar, FBaseLightProperties& Properties)
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

    FBaseLightProperties GetBaseLightProperties() const;
    void CopyFrom(const FBaseLightProperties& Properties);
    void CopyBasicData(const FBaseLightProperties& Properties);
};

USTRUCT(BlueprintType, DisplayName = "Point Light Properties")
struct GTCORE_API FPointLightProperties : public FBaseLightProperties
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

    FPointLightProperties()
        : SourceRadius(0.0f)
        , SoftSourceRadius(0.0f)
        , SourceLength(0.0f)
        , bUseInverseSquaredFalloff(false)
        , LightFalloffExponent(8.0f)
    {
        bUseInverseSquaredFalloff = false;
    }
    
    friend FArchive& operator<<(FArchive& Ar, FPointLightProperties& Properties)
    {
        Ar << Properties.SourceRadius;
        Ar << Properties.SoftSourceRadius;
        Ar << Properties.SourceLength;
        Ar << Properties.bUseInverseSquaredFalloff;
        Ar << Properties.LightFalloffExponent;
        return Ar;
    }
};

USTRUCT(BlueprintType, DisplayName = "Spot Light Properties")
struct GTCORE_API FSpotLightProperties : public FBaseLightProperties
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

    FSpotLightProperties()
        : InnerConeAngle(0.0f)
        , OuterConeAngle(44.0f)
        , SourceRadius(0.0f)
        , SoftSourceRadius(0.0f)
        , SourceLength(0.0f)
        , bUseInverseSquaredFalloff(false)
        , LightFalloffExponent(8.0f)
    {
        bUseInverseSquaredFalloff = false;
    }
    
    friend FArchive& operator<<(FArchive& Ar, FSpotLightProperties& Properties)
    {
        Ar << Properties.InnerConeAngle;
        Ar << Properties.OuterConeAngle;
        Ar << Properties.SourceLength;
        Ar << Properties.bUseInverseSquaredFalloff;
        Ar << Properties.LightFalloffExponent;
        return Ar;
    }
};

USTRUCT(BlueprintType, DisplayName = "Rect Light Properties")
struct GTCORE_API FRectLightProperties : public FBaseLightProperties
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

    FRectLightProperties()
        : SourceWidth(64.0f)
        , SourceHeight(64.0f)
        , BarnDoorAngle(88.0f)
        , BarnDoorLength(20.0f)
        , SourceTexture(nullptr)
    {}
    
    friend FArchive& operator<<(FArchive& Ar, FRectLightProperties& Properties)
    {
        Ar << Properties.SourceWidth;
        Ar << Properties.SourceHeight;
        Ar << Properties.BarnDoorAngle;
        Ar << Properties.BarnDoorLength;
        Ar << Properties.SourceTexture;
        return Ar;
    }
};

UCLASS(DisplayName = "Lighting Function Library")
class GTCORE_API ULightingDataLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light|Properties", DisplayName = "Base Light Properties (From Point)", meta = (BlueprintAutocast = true, CompactNodeTitle = "->"))
        static FBaseLightProperties GetBaseProperties_Point(const FPointLightProperties& Target);

    UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light|Properties", DisplayName = "Base Light Properties (From Spot)", meta = (BlueprintAutocast = true, CompactNodeTitle = "->"))
        static FBaseLightProperties GetBaseProperties_Spot(const FSpotLightProperties& Target);

    UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light|Properties", DisplayName = "Base Light Properties (From Rect)", meta = (BlueprintAutocast = true, CompactNodeTitle = "->"))
        static FBaseLightProperties GetBaseProperties_Rect(const FRectLightProperties& Target);

    UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light|Properties", DisplayName = "Point Light Properties (From Base)", meta = (BlueprintAutocast = true, CompactNodeTitle = "->"))
        static FPointLightProperties GetPointProperties(const FBaseLightProperties& Target, const bool bOnlyBasicData);

    UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light|Properties", DisplayName = "Spot Light Properties (From Base)", meta = (BlueprintAutocast = true, CompactNodeTitle = "->"))
        static FSpotLightProperties GetSpotProperties(const FBaseLightProperties& Target, const bool bOnlyBasicData);

    UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light|Properties", DisplayName = "Rect Light Properties (From Base)", meta = (BlueprintAutocast = true, CompactNodeTitle = "->"))
        static FRectLightProperties GetRectProperties(const FBaseLightProperties& Target, const bool bOnlyBasicData);
	
	UFUNCTION(BlueprintCallable, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static void SetLightDrawDistanceSettings(ULightComponent* Target, const FLightDrawDistance& Settings);

	UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static FLightDrawDistance GetLightDrawDistanceSettings(const ULightComponent* Target);

	UFUNCTION(BlueprintCallable, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static void SetBaseLightProperties(ULightComponent* Target, const FBaseLightProperties& Properties);

	UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static FBaseLightProperties GetBaseLightProperties(const ULightComponent* Target);

	UFUNCTION(BlueprintCallable, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static void SetPointLightProperties(UPointLightComponent* Target, const FPointLightProperties& Properties);

	UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static FPointLightProperties GetPointLightProperties(const UPointLightComponent* Target);

	UFUNCTION(BlueprintCallable, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static void SetSpotLightProperties(USpotLightComponent* Target, const FSpotLightProperties& Properties);

	UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static FSpotLightProperties GetSpotLightProperties(const USpotLightComponent* Target);

	UFUNCTION(BlueprintCallable, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static void SetRectLightProperties(URectLightComponent* Target, const FRectLightProperties& Properties);

	UFUNCTION(BlueprintPure, Category = "Rendering|Components|Light", meta = (DefaultToSelf = "Target"))
		static FRectLightProperties GetRectLightProperties(const URectLightComponent* Target);
};
