// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "VisionConeComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GTRUNTIME_API UVisionConeComponent final : public USceneComponent
{
	GENERATED_BODY()

public:

	UVisionConeComponent();

	UPROPERTY(EditAnywhere, Category = "Settings|VisionCone")
		ECollisionChannel TraceChannel;

	UPROPERTY(EditAnywhere, Category = "Settings|VisionCone", meta = (ClampMin = 10.0f, UIMin = 10.0f))
		float MaxDistance;

	UPROPERTY(EditAnywhere, Category = "Settings|VisionCone", meta = (ClampMin = 10.0f, UIMin = 10.0f, ClampMax = 90.0f, UIMax = 90.0f))
		float BaseAngle;

	UPROPERTY(EditAnywhere, Category = "Settings|VisionCone", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float PeripheralAngle;

	UFUNCTION(BlueprintPure, Category = "VisionCone")
		bool GetTraceTo(const AActor* InActor) const;
	
	UFUNCTION(BlueprintPure, Category = "VisionCone")
		float GetAngleTo(const AActor* InActor) const;
	
	UFUNCTION(BlueprintPure, Category = "VisionCone")
		bool IsActorInRange(const AActor* InActor) const;
		
	UFUNCTION(BlueprintPure, Category = "VisionCone")
		bool IsActorInVision(const AActor* InActor) const;
		
	UFUNCTION(BlueprintPure, Category = "VisionCone")
		bool IsActorInPeripheral(const AActor* InActor) const;

	float GetBaseAngle() const { return FMath::Min(BaseAngle, 90.0f); }
	float GetPeripheralAngle() const { return FMath::Min(GetBaseAngle() + PeripheralAngle, 90.0f); }
};
