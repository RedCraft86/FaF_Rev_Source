// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "ComponentVisualizer.h"
#include "Components/VisionConeComponent.h"
#include "ComponentVisualizers/VisualizerHelpers.h"

class FVisionConeVisualizer final : public FComponentVisualizer
{
public:
	
	static TSharedPtr<FComponentVisualizer> MakeInstance()
	{
		return MakeShareable(new FVisionConeVisualizer);
	}
	
private:

	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override
	{
		if (const UVisionConeComponent* ConeComponent = Cast<const UVisionConeComponent>(Component))
		{
			DrawWireSphereCappedCone(PDI, ConeComponent->GetComponentTransform(), ConeComponent->MaxDistance,
				ConeComponent->GetBaseAngle(), 16, 4, 8, FLinearColor::Red, SDPG_World);

			DrawWireSphereCappedCone(PDI, ConeComponent->GetComponentTransform(), ConeComponent->MaxDistance,
				ConeComponent->GetPeripheralAngle(), 16, 4, 8, FLinearColor::Yellow, SDPG_World);
		}
	}

	virtual void DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override
	{
		if (const UVisionConeComponent* ConeComponent = Cast<const UVisionConeComponent>(Component))
		{
			VisualizerHelpers::DrawText(View, Canvas, ConeComponent->GetComponentLocation(),
				UEnum::GetDisplayValueAsText<ECollisionChannel>(ConeComponent->TraceChannel),
				12, FLinearColor::Green, true);
		}
	}
};
