// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "Characters/PlayerSensingComponent.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "ComponentVisualizer.h"

class EDITORCORE_API FPlayerSensingVisualizer final : public FComponentVisualizer
{
public:

	static TSharedPtr<FComponentVisualizer> MakeInstance()
	{
		return MakeShareable(new FPlayerSensingVisualizer);
	}

private:

	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override
	{
		if (const UPlayerSensingComponent* SenseComponent = Cast<UPlayerSensingComponent>(Component))
		{
			if (SenseComponent->MaxDistance >= 10.0f && SenseComponent->OuterAngle >= 10.0f &&
				SenseComponent->InnerAngle >= 5.0f)
			{
				DrawWireSphereCappedCone(PDI, SenseComponent->GetComponentTransform(), SenseComponent->MaxDistance,
					SenseComponent->OuterAngle, 12, 3, 16, FColor::Yellow, SDPG_World);
		
				TArray<FVector> Verts;
				DrawWireCone(PDI, Verts, SenseComponent->GetComponentTransform(), SenseComponent->MaxDistance,
					SenseComponent->InnerAngle, 12, FColor::Green, SDPG_World);
			}
		}
	}

	virtual void DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override
	{
		if (const UPlayerSensingComponent* SenseComponent = Cast<UPlayerSensingComponent>(Component))
		{
			if (SenseComponent->MaxDistance >= 10.0f && SenseComponent->OuterAngle >= 10.0f
				&& SenseComponent->InnerAngle >= 5.0f)
			{
				FVector2D ScreenPos;
				if (FSceneView::ProjectWorldToScreen(SenseComponent->GetComponentLocation(), View->UnconstrainedViewRect,
					View->ViewMatrices.GetViewProjectionMatrix(), ScreenPos))
				{
					ScreenPos /= Canvas->GetDPIScale();
		
					const FString Str = FString::Printf(TEXT("SensingEnabled: %s%s"), *LexToString(SenseComponent->bEnabled),
						SenseComponent->bDetectOnlyMovement ? TEXT("\nDetectOnlyMovement") : TEXT(""));
					
					FCanvasTextItem Text(ScreenPos, FText::FromString(Str),
						FSlateFontInfo(GEngine->GetSmallFont(), 10), FColor::Cyan);
				
					Text.EnableShadow(FColor::Black);
				
					Canvas->DrawItem(Text);
				}
			}
		}
	}
};