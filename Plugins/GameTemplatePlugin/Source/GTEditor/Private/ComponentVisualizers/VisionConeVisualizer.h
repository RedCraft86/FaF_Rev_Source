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
			if (FMath::IsNearlyZero(ConeComponent->PeripheralAngle) || FMath::IsNearlyEqual(ConeComponent->GetBaseAngle(), 90.0f))
			{
				DrawWireSphereCappedCone(PDI, ConeComponent->GetComponentTransform(), ConeComponent->MaxDistance,
					ConeComponent->GetBaseAngle(), 16, 4, 16, FLinearColor::Red, SDPG_World);	
			}
			else
			{
				TArray<FVector> Verts;
				DrawWireCone(PDI, Verts, ConeComponent->GetComponentTransform(), ConeComponent->MaxDistance,
					ConeComponent->GetBaseAngle(), 16, FLinearColor::Red, SDPG_World);
				DrawWireSphereCappedCone(PDI, ConeComponent->GetComponentTransform(), ConeComponent->MaxDistance,
					ConeComponent->GetPeripheralAngle(), 16, 4, 16, FLinearColor::Yellow, SDPG_World);	
			}
		}
	}

	virtual void DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override
	{
		if (const UVisionConeComponent* ConeComponent = Cast<const UVisionConeComponent>(Component))
		{
			VisualizerHelpers::DrawText(View, Canvas, ConeComponent->GetComponentLocation(), FText::Format(INVTEXT("Trace: {0}"), 
				UEnum::GetDisplayValueAsText<ECollisionChannel>(ConeComponent->TraceChannel)),
				12, FLinearColor::Green, true);
		}
	}
};
