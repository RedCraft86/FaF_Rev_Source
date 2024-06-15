// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "DebugShapesComponent.generated.h"

USTRUCT(BlueprintType)
struct FDebugPointData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugPointData")
        FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugPointData", meta = (ClampMin = 1.0f, UIMin = 1.0f))
        float Size;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugPointData")
        FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugPointData")
        bool bRenderOnTopOfEverything;

    FDebugPointData()
        : Location(FVector(0.0f))
        , Size(2.0f)
        , Color(FColor::Cyan)
        , bRenderOnTopOfEverything(false)
    {}
};

USTRUCT(BlueprintType)
struct FDebugLineData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugLineData")
        FVector Start;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugLineData")
        FVector End;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugLineData", meta = (ClampMin = 1.0f, UIMin = 1.0f))
        float Thickness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugLineData")
        FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugLineData")
        bool bRenderOnTopOfEverything;

    FDebugLineData()
        : Start(FVector(0.0f))
        , End(FVector(0.0f))
        , Thickness(1.0f)
        , Color(FColor::Cyan)
        , bRenderOnTopOfEverything(false)
    {}
};

USTRUCT(BlueprintType)
struct FDebugCircleData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCircleData")
        FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCircleData")
        FRotator Rotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCircleData", meta = (ClampMin = 1.0f, UIMin = 1.0f))
        float Radius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCircleData", meta = (ClampMin = 1, UIMin = 1))
        uint8 NumOfSides;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCircleData", meta = (ClampMin = 1.0f, UIMin = 1.0f))
        float Thickness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCircleData")
        FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCircleData")
        bool bRenderOnTopOfEverything;

    FDebugCircleData()
        : Location(FVector(0.0f))
        , Rotation(FRotator(0.0f))
        , Radius(32.0f)
        , NumOfSides(16)
        , Thickness(1.0f)
        , Color(FColor::Cyan)
        , bRenderOnTopOfEverything(false)
    {}
};

USTRUCT(BlueprintType)
struct FDebugArcData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugArcData")
        FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugArcData")
        FRotator Rotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugArcData", meta = (ClampMin = -360.0f, UIMin = -360.0f, ClampMax = 360.0f, UIMax = 360.0f))
        float MinAngle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugArcData", meta = (ClampMin = -360.0f, UIMin = -360.0f, ClampMax = 360.0f, UIMax = 360.0f))
        float MaxAngle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugArcData", meta = (ClampMin = 1, UIMin = 1))
        float Radius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugArcData", meta = (ClampMin = 1.0f, UIMin = 1.0f))
        uint8 Sections;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugArcData", meta = (ClampMin = 1.0f, UIMin = 1.0f))
        float Thickness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugArcData")
        FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugArcData")
        bool bRenderOnTopOfEverything;

    FDebugArcData()
        : Location(FVector(0.0f))
        , Rotation(FRotator(0.0f))
        , MinAngle(0.0f)
        , MaxAngle(90.0f)
        , Radius(32.0f)
        , Sections(8)
        , Thickness(1.0f)
        , Color(FColor::Cyan)
        , bRenderOnTopOfEverything(false)
    {}
};

USTRUCT(BlueprintType)
struct FDebugBoxData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugBoxData")
        FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugBoxData")
        FRotator Rotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugBoxData", meta = (ClampMin = 1.0f, UIMin = 1.0f))
        FVector Extents;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugBoxData", meta = (ClampMin = 1.0f, UIMin = 1.0f))
        float Thickness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugBoxData")
        FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugBoxData")
        bool bRenderOnTopOfEverything;

    FDebugBoxData()
        : Location(FVector(0.0f))
        , Rotation(FRotator(0.0f))
        , Extents(FVector(32.0f))
        , Thickness(1.0f)
        , Color(FColor::Cyan)
        , bRenderOnTopOfEverything(false)
    {}
};

USTRUCT(BlueprintType)
struct FDebugSphereData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugSphereData")
        FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugSphereData", meta = (ClampMin = 1.0f, UIMin = 1.0f))
        float Radius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugSphereData", meta = (ClampMin = 1, UIMin = 1))
        uint8 NumOfSides;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugSphereData", meta = (ClampMin = 1.0f, UIMin = 1.0f))
        float Thickness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugSphereData")
        FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugSphereData")
        bool bRenderOnTopOfEverything;

    FDebugSphereData()
        : Location(FVector(0.0f))
        , Radius(32.0f)
        , NumOfSides(16)
        , Thickness(1.0f)
        , Color(FColor::Cyan)
        , bRenderOnTopOfEverything(false)
    {}
};

USTRUCT(BlueprintType)
struct FDebugCylinderData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCylinderData")
        FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCylinderData")
        FRotator Rotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCylinderData", meta = (ClampMin = 1.0f, UIMin = 1.0f))
        float Radius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCylinderData", meta = (ClampMin = 1.0f, UIMin = 1.0f))
        float HalfHeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCylinderData", meta = (ClampMin = 1, UIMin = 1))
        uint8 NumOfSides;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCylinderData", meta = (ClampMin = 1.0f, UIMin = 1.0f))
        float Thickness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCylinderData")
        FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCylinderData")
        bool bRenderOnTopOfEverything;

    FDebugCylinderData()
        : Location(FVector(0.0f))
        , Rotation(FRotator(0.0f))
        , Radius(32.0f)
        , HalfHeight(64.0f)
        , NumOfSides(12)
        , Thickness(1.0f)
        , Color(FColor::Cyan)
        , bRenderOnTopOfEverything(false)
    {}
};

USTRUCT(BlueprintType)
struct FDebugCapsuleData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCapsuleData")
        FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCapsuleData")
        FRotator Rotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCapsuleData", meta = (ClampMin = 1.0f, UIMin = 1.0f))
        float Radius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCapsuleData", meta = (ClampMin = 1.0f, UIMin = 1.0f))
        float HalfHeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCapsuleData", meta = (ClampMin = 1, UIMin = 1))
        uint8 NumOfSides;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCapsuleData", meta = (ClampMin = 1.0f, UIMin = 1.0f))
        float Thickness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCapsuleData")
        FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugCapsuleData")
        bool bRenderOnTopOfEverything;

    FDebugCapsuleData()
        : Location(FVector(0.0f))
        , Rotation(FRotator(0.0f))
        , Radius(32.0f)
        , HalfHeight(64.0f)
        , NumOfSides(16)
        , Thickness(1.0f)
        , Color(FColor::Cyan)
        , bRenderOnTopOfEverything(false)
    {}
};

USTRUCT(BlueprintType)
struct FDebugStringData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugStringData")
        FString String;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugStringData")
        FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugStringData")
        FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugStringData", meta = (ClampMin = 1, UIMin = 1))
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

UCLASS(MinimalAPI, NotBlueprintable, ClassGroup = (Debugging), meta = (BlueprintSpawnableComponent))
class UDebugShapesComponent final : public UActorComponent
{
	GENERATED_BODY()

public:

	UDebugShapesComponent();

#if WITH_EDITORONLY_DATA
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
        TMap<FName, FDebugPointData> DebugPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
        TMap<FName, FDebugLineData> DebugLines;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
        TMap<FName, FDebugCircleData> DebugCircles;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
        TMap<FName, FDebugArcData> DebugArcs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
        TMap<FName, FDebugBoxData> DebugBoxes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
        TMap<FName, FDebugSphereData> DebugSpheres;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
        TMap<FName, FDebugCylinderData> DebugCylinders;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
        TMap<FName, FDebugCapsuleData> DebugCapsules;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
        TMap<FName, FDebugStringData> DebugStrings;

    GTRUNTIME_API FVector TransformLocation(const FVector& LocalLocation) const;
    GTRUNTIME_API FRotator TransformRotation(const FRotator& LocalRotation) const;
    GTRUNTIME_API FVector AsForwardVector(const FRotator& LocalRotation) const;
    GTRUNTIME_API FVector AsRightVector(const FRotator& LocalRotation) const;
    GTRUNTIME_API FVector AsUpVector(const FRotator& LocalRotation) const;
#endif

protected:

	virtual void BeginPlay() override;
};
