// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "RCDebuggingTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ActorComponent.h"
#include "RCDebugShapesVisualizer.generated.h"

UCLASS(NotBlueprintable, ClassGroup = (Editor), meta = (BlueprintSpawnableComponent))
class RCDEBUGGING_API URCDebugShapeVisualizer final : public UActorComponent
{
	GENERATED_BODY()

public:	

	URCDebugShapeVisualizer()
	{
		PrimaryComponentTick.bCanEverTick = false;
		PrimaryComponentTick.bStartWithTickEnabled = false;
		bIsEditorOnly = true;
#if WITH_EDITORONLY_DATA
		SetIsVisualizationComponent(true);
#endif
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		TMap<FName, FWirePointData> DebugPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		TMap<FName, FWireLineData> DebugLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		TMap<FName, FWireCircleData> DebugCircles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		TMap<FName, FWireArcData> DebugArcs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		TMap<FName, FWireBoxData> DebugBoxes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		TMap<FName, FWireSphereData> DebugSpheres;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		TMap<FName, FWireCylinderData> DebugCylinders;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		TMap<FName, FWireConeData> DebugCones;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		TMap<FName, FWireCapsuleData> DebugCapsules;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		TMap<FName, FDebugStringData> DebugStrings;

	FVector TransformLocation(const FVector& LocalLocation) const
	{
		return UKismetMathLibrary::TransformLocation(GetOwner()->GetActorTransform(), LocalLocation);
	}
	
	FRotator TransformRotation(const FRotator& LocalRotation) const
	{
		return UKismetMathLibrary::TransformRotation(GetOwner()->GetActorTransform(), LocalRotation);
	}
	
	FVector AsForwardVector(const FRotator& LocalRotation) const
	{
		return UKismetMathLibrary::GetForwardVector(TransformRotation(LocalRotation));
	}
	
	FVector AsRightVector(const FRotator& LocalRotation) const
	{
		return UKismetMathLibrary::GetRightVector(TransformRotation(LocalRotation));
	}
	
	FVector AsUpVector(const FRotator& LocalRotation) const
	{
		return UKismetMathLibrary::GetUpVector(TransformRotation(LocalRotation));
	}
};
