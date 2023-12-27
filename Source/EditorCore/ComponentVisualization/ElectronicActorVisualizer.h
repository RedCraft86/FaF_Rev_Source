// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "Electrical/GCElectricalActor.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "ComponentVisualizer.h"

class EDITORCORE_API FElectronicActorVisualizer final : public FComponentVisualizer
{
public:

	static TSharedPtr<FComponentVisualizer> MakeInstance()
	{
		return MakeShareable(new FElectronicActorVisualizer);
	}

private:

	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override
	{
		if (const UGCElectronicVisualizer* VisComponent = Cast<UGCElectronicVisualizer>(Component))
		{
			const TArray<FVector> NextLinks = VisComponent->GetNextTargets();
			for (const FVector& Link : NextLinks)
			{
				PDI->DrawLine(VisComponent->GetOwner()->GetActorLocation(), Link,
					FLinearColor::Green, SDPG_Foreground, 2);
			}

			const TArray<FVector> LastLinks = VisComponent->GetLastTargets();
			for (const FVector& Link : LastLinks)
			{
				PDI->DrawLine(VisComponent->GetOwner()->GetActorLocation(), Link,
					FLinearColor::Yellow, SDPG_Foreground, 2);
			}
		}
	}

	virtual void DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override
	{
		// if (const UGCElectronicVisualizer* VisComponent = Cast<UGCElectronicVisualizer>(Component))
		// {
		// }
	}
};