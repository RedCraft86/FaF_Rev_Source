// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "RCDebuggingTypes.generated.h"

USTRUCT(BlueprintType)
struct RCDEBUGGING_API FWirePointData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WirePointData")
        FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WirePointData")
        float Size;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WirePointData")
        FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WirePointData")
        bool RenderOnTopOfEverything;

    FWirePointData()
        : Location(FVector(0.0f))
        , Size(2.0f)
        , Color(FColor::Cyan)
        , RenderOnTopOfEverything(false)
    {}
};

USTRUCT(BlueprintType)
struct RCDEBUGGING_API FWireLineData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireLineData")
        FVector Start;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireLineData")
        FVector End;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireLineData")
        float Thickness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireLineData")
        FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireLineData")
        bool RenderOnTopOfEverything;

    FWireLineData()
        : Start(FVector(0.0f))
        , End(FVector(0.0f))
        , Thickness(1.0f)
        , Color(FColor::Cyan)
        , RenderOnTopOfEverything(false)
    {}
};

USTRUCT(BlueprintInternalUseOnly)
struct RCDEBUGGING_API FWirePrimitiveData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WirePrimitiveData")
        FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WirePrimitiveData")
        FRotator Rotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WirePrimitiveData")
        float Thickness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WirePrimitiveData")
        FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WirePrimitiveData")
        bool RenderOnTopOfEverything;

    FWirePrimitiveData()
        : Location(FVector::ZeroVector)
        , Rotation(FRotator::ZeroRotator)
        , Thickness(1.0f)
        , Color(FColor::Cyan)
        , RenderOnTopOfEverything(false)
    {}
};

USTRUCT(BlueprintType)
struct RCDEBUGGING_API FWireCircleData : public FWirePrimitiveData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireCircleData")
        float Radius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireCircleData")
        uint8 NumOfSides;

    FWireCircleData()
        : Radius(32.0f)
        , NumOfSides(16)
    {}
};

USTRUCT(BlueprintType)
struct RCDEBUGGING_API FWireArcData : public FWirePrimitiveData
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireArcData")
        float MinAngle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireArcData")
        float MaxAngle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireArcData")
        float Radius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireArcData")
        uint8 Sections;

    FWireArcData()
        : MinAngle(0.0f)
        , MaxAngle(90.0f)
        , Radius(32.0f)
        , Sections(8)
    {}
};

USTRUCT(BlueprintType)
struct RCDEBUGGING_API FWireBoxData : public FWirePrimitiveData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireBoxData")
        FVector Extents;

    FWireBoxData()
        : Extents(FVector(32.0f))
    {}
};

USTRUCT(BlueprintType)
struct RCDEBUGGING_API FWireSphereData : public FWirePrimitiveData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireSphereData")
        float Radius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireSphereData")
        uint8 NumOfSides;

    FWireSphereData()
        : Radius(32.0f)
        , NumOfSides(16)
    {}
};

USTRUCT(BlueprintType)
struct RCDEBUGGING_API FWireCylinderData : public FWirePrimitiveData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireCylinderData")
        float Radius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireCylinderData")
        float HalfHeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireCylinderData")
        uint8 NumOfSides;

    FWireCylinderData()
        : Radius(32.0f)
        , HalfHeight(64.0f)
        , NumOfSides(12)
    {}
};

USTRUCT(BlueprintType)
struct RCDEBUGGING_API FWireConeData : public FWirePrimitiveData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireConeData")
        float Length;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireConeData")
        float Angle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireConeData")
        uint8 NumOfSides;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireConeData")
        bool bDrawCap;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireConeData", meta = (EditCondition = "bDrawCap"))
        uint8 CapArcFrequency;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireConeData", meta = (EditCondition = "bDrawCap"))
        uint8 NumOfCapArcs;

    FWireConeData()
        : Length(250.0f)
        , Angle(45.0f)
        , NumOfSides(12)
        , bDrawCap(true)
        , CapArcFrequency(3)
        , NumOfCapArcs(8)
    {}
};

USTRUCT(BlueprintType)
struct RCDEBUGGING_API FWireCapsuleData : public FWirePrimitiveData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireCapsuleData")
        float Radius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireCapsuleData")
        float HalfHeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WireCapsuleData")
        uint8 NumOfSides;

    FWireCapsuleData()
        : Radius(32.0f)
        , HalfHeight(64.0f)
        , NumOfSides(16)
    {}
};

USTRUCT(BlueprintType)
struct RCDEBUGGING_API FDebugStringData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugStringData")
        FString String;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugStringData")
        FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugStringData")
        FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugStringData")
        uint8 FontScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugStringData")
        bool bDrawShadow;

    FDebugStringData()
        : String(TEXT("Debug_String"))
        , Location(FVector(0.0f))
        , Color(FColor::Green)
        , FontScale(12)
        , bDrawShadow(true)
    {}
};