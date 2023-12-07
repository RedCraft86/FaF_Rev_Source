// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "Gameplay/TaskActors/GCTaskActorManager.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "ComponentVisualizer.h"

class EDITORCORE_API FTaskActorManagerVisualizer final : public FComponentVisualizer
{
public:

	static TSharedPtr<FComponentVisualizer> MakeInstance()
	{
		return MakeShareable(new FTaskActorManagerVisualizer);
	}

private:

	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override
	{
		if (const UGCTaskActorManagerComponent* VisComponent = Cast<UGCTaskActorManagerComponent>(Component))
		{
			for (const TPair<FVector, FLinearColor>& Target : VisComponent->Targets)
			{
				PDI->DrawLine(VisComponent->GetOwner()->GetActorLocation(), Target.Key,
					Target.Value, SDPG_Foreground, 1);
			}
		}
	}

	virtual void DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override
	{
		if (const UGCTaskActorManagerComponent* VisComponent = Cast<UGCTaskActorManagerComponent>(Component))
		{
			FVector2D ScreenPos;
			if (FSceneView::ProjectWorldToScreen(VisComponent->GetOwner()->GetActorLocation(), View->UnconstrainedViewRect,
				View->ViewMatrices.GetViewProjectionMatrix(), ScreenPos))
			{
				ScreenPos /= Canvas->GetDPIScale();
				FCanvasTextItem Text(ScreenPos, FText::FromString(FString::Printf(TEXT("Managing %d Actors"), VisComponent->Targets.Num())),
					FSlateFontInfo(GEngine->GetSmallFont(), 12), FColor::Green);
				
				Text.EnableShadow(FColor::Black);
				
				Canvas->DrawItem(Text);
			}
		}
	}
};