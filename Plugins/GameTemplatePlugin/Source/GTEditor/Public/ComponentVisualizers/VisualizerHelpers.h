// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "CanvasItem.h"
#include "CanvasTypes.h"

namespace VisualizerHelpers
{
	GTEDITOR_API inline void DrawArcWithThickness(FPrimitiveDrawInterface* PDI, const FVector Base, const FVector X, const FVector Y, const float MinAngle, const float MaxAngle, const float Radius, const int Sections, const FLinearColor& Color, uint8 DepthPriority, const float Thickness)
	{
		const float AngleStep = (MaxAngle - MinAngle) / Sections;
		float CurrentAngle = MinAngle;

		FVector LastVertex = Base + Radius * (FMath::Cos(CurrentAngle * (PI / 180.0f)) * X + FMath::Sin(CurrentAngle * (PI / 180.0f)) * Y);
		CurrentAngle += AngleStep;

		for (int32 i = 0; i < Sections; i++)
		{
			FVector ThisVertex = Base + Radius * (FMath::Cos(CurrentAngle * (PI / 180.0f)) * X + FMath::Sin(CurrentAngle * (PI / 180.0f)) * Y);

			if (i == 0)
			{
				PDI->DrawTranslucentLine(Base, LastVertex, Color, DepthPriority, Thickness);
			}
			else if (i == Sections - 1)
			{
				PDI->DrawTranslucentLine(Base, ThisVertex, Color, DepthPriority, Thickness);
			}

			PDI->DrawTranslucentLine(LastVertex, ThisVertex, Color, DepthPriority, Thickness);
			LastVertex = ThisVertex;
			CurrentAngle += AngleStep;
		}
	}

	GTEDITOR_API inline void DrawText(const FSceneView* View, FCanvas* Canvas, const FVector& Location, const FText& Text, const uint8 FontScale = 12, const FLinearColor& Color = FLinearColor::White, const bool bDrawShadow = false, const UFont* Font = nullptr)
	{
		FVector2D ScreenPos;
		if (FSceneView::ProjectWorldToScreen(Location, View->UnconstrainedViewRect, View->ViewMatrices.GetViewProjectionMatrix(), ScreenPos))
		{
			ScreenPos /= Canvas->GetDPIScale();
			FCanvasTextItem TextItem = FCanvasTextItem(ScreenPos, Text, {Font ? Font : GEngine->GetSmallFont(), FontScale > 0 ? (float)FontScale : 12.0f, NAME_None}, Color);
			bDrawShadow ? TextItem.EnableShadow(FColor::Black) : TextItem.DisableShadow();
			Canvas->DrawItem(TextItem);
		}
	}
};