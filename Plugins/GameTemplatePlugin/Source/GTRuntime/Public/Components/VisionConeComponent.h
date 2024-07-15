// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "VisionConeComponent.generated.h"

UENUM(BlueprintType)
enum class EVisionConeState : uint8
{
	None,
	Peripheral,
	FullySeen
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GTRUNTIME_API UVisionConeComponent final : public USceneComponent
{
	GENERATED_BODY()

public:

	UVisionConeComponent();

	UPROPERTY(EditAnywhere, Category = "Settings|VisionCone")
		TEnumAsByte<ECollisionChannel> TraceChannel;

	UPROPERTY(EditAnywhere, Category = "Settings|VisionCone", meta = (ClampMin = 10.0f, UIMin = 10.0f))
		float MaxDistance;

	UPROPERTY(EditAnywhere, Category = "Settings|VisionCone", meta = (ClampMin = 10.0f, UIMin = 10.0f, ClampMax = 90.0f, UIMax = 90.0f))
		float BaseAngle;

	UPROPERTY(EditAnywhere, Category = "Settings|VisionCone", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float PeripheralAngle;

	UFUNCTION(BlueprintPure, Category = "VisionCone")
		bool GetTraceTo(const AActor* InActor, const bool bCollidingOnly = false) const;
	
	UFUNCTION(BlueprintPure, Category = "VisionCone")
		float GetAngleTo(const AActor* InActor) const;
	
	UFUNCTION(BlueprintPure, Category = "VisionCone")
		bool IsActorInRange(const AActor* InActor) const;

	UFUNCTION(BlueprintPure, Category = "VisionCone")
		EVisionConeState GetActorVisionState(const AActor* InActor, const bool bCollidingOnly = false) const;

	float GetBaseAngle() const { return FMath::Min(BaseAngle, 90.0f); }
	float GetPeripheralAngle() const { return FMath::Min(GetBaseAngle() + PeripheralAngle, 90.0f); }
};
