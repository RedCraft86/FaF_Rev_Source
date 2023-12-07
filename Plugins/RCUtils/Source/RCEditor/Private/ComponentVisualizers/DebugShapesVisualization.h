// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "RCDebugShapesVisualizer.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "RCEdHelpers.h"
#include "ComponentVisualizer.h"

class RCEDITOR_API FDebugComponentVisualizer final : public FComponentVisualizer
{
public:

	static TSharedPtr<FComponentVisualizer> MakeInstance()
	{
		return MakeShareable(new FDebugComponentVisualizer);
	}

private:

	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override
	{
		if (const URCDebugShapeVisualizer* VizComponent = Cast<URCDebugShapeVisualizer>(Component))
		{
			for (const TPair<FName, FWirePointData>& Point : VizComponent->DebugPoints)
			{
				PDI->DrawPoint(VizComponent->TransformLocation(Point.Value.Location), Point.Value.Color,
					Point.Value.Size, Point.Value.RenderOnTopOfEverything ? SDPG_Foreground : SDPG_World);
			}

			for (const TPair<FName, FWireLineData>& Line : VizComponent->DebugLines)
			{
				PDI->DrawLine(VizComponent->TransformLocation(Line.Value.Start),
					VizComponent->TransformLocation(Line.Value.End), Line.Value.Color,
					Line.Value.RenderOnTopOfEverything ? SDPG_Foreground : SDPG_World, Line.Value.Thickness);
			}

			for (const TPair<FName, FWireCircleData>& Circle : VizComponent->DebugCircles)
			{
				DrawCircle(PDI, VizComponent->TransformLocation(Circle.Value.Location),
					VizComponent->AsForwardVector(Circle.Value.Rotation),
					VizComponent->AsRightVector(Circle.Value.Rotation),
					Circle.Value.Color, Circle.Value.Radius, Circle.Value.NumOfSides,
					Circle.Value.RenderOnTopOfEverything ? SDPG_Foreground : SDPG_World, Circle.Value.Thickness);
			}

			for (const TPair<FName, FWireArcData>& Arc : VizComponent->DebugArcs)
			{
				RCEdHelpers::DrawArcWithThickness(PDI, VizComponent->TransformLocation(Arc.Value.Location),
					VizComponent->AsForwardVector(Arc.Value.Rotation),
					VizComponent->AsRightVector(Arc.Value.Rotation),
					Arc.Value.MinAngle, Arc.Value.MaxAngle, Arc.Value.Radius, Arc.Value.Sections, Arc.Value.Color,
					Arc.Value.RenderOnTopOfEverything ? SDPG_Foreground : SDPG_World, Arc.Value.Thickness);
			}

			for (const TPair<FName, FWireBoxData>& Box : VizComponent->DebugBoxes)
			{
				DrawOrientedWireBox(PDI, VizComponent->TransformLocation(Box.Value.Location),
					VizComponent->AsForwardVector(Box.Value.Rotation),
					VizComponent->AsRightVector(Box.Value.Rotation),
					VizComponent->AsUpVector(Box.Value.Rotation),
					Box.Value.Extents, Box.Value.Color,
					Box.Value.RenderOnTopOfEverything ? SDPG_Foreground : SDPG_World, Box.Value.Thickness);
			}

			for (const TPair<FName, FWireSphereData>& Sphere : VizComponent->DebugSpheres)
			{
				DrawWireSphere(PDI, VizComponent->TransformLocation(Sphere.Value.Location),
					Sphere.Value.Color, Sphere.Value.Radius, Sphere.Value.NumOfSides,
					Sphere.Value.RenderOnTopOfEverything ? SDPG_Foreground : SDPG_World, Sphere.Value.Thickness);
			}

			for (const TPair<FName, FWireCylinderData>& Cylinder : VizComponent->DebugCylinders)
			{
				DrawWireCylinder(PDI, VizComponent->TransformLocation(Cylinder.Value.Location),
					VizComponent->AsForwardVector(Cylinder.Value.Rotation),
					VizComponent->AsRightVector(Cylinder.Value.Rotation),
					VizComponent->AsUpVector(Cylinder.Value.Rotation),
					Cylinder.Value.Color, Cylinder.Value.Radius, Cylinder.Value.HalfHeight, Cylinder.Value.NumOfSides,
					Cylinder.Value.RenderOnTopOfEverything ? SDPG_Foreground : SDPG_World, Cylinder.Value.Thickness);
			}

			for (const TPair<FName, FWireConeData>& Cone : VizComponent->DebugCones)
			{
				FTransform Transform(VizComponent->TransformRotation(Cone.Value.Rotation),
					VizComponent->TransformLocation(Cone.Value.Location));
				
				if (Cone.Value.bDrawCap)
				{
					RCEdHelpers::DrawCappedConeWithThickness(PDI, Transform, Cone.Value.Length, Cone.Value.Angle,
						Cone.Value.NumOfSides, Cone.Value.CapArcFrequency, Cone.Value.NumOfCapArcs, Cone.Value.Color,
						Cone.Value.RenderOnTopOfEverything ? SDPG_Foreground : SDPG_World, Cone.Value.Thickness);
				}
				else
				{
					TArray<FVector> Verts;
					DrawWireCone(PDI, Verts, Transform, Cone.Value.Length, Cone.Value.Angle, Cone.Value.NumOfSides,
						Cone.Value.Color, Cone.Value.RenderOnTopOfEverything ? SDPG_Foreground : SDPG_World, Cone.Value.Thickness);
				}
			}

			for (const TPair<FName, FWireCapsuleData>& Capsule : VizComponent->DebugCapsules)
			{
				DrawWireCapsule(PDI, VizComponent->TransformLocation(Capsule.Value.Location),
					VizComponent->AsForwardVector(Capsule.Value.Rotation),
					VizComponent->AsRightVector(Capsule.Value.Rotation),
					VizComponent->AsUpVector(Capsule.Value.Rotation),
					Capsule.Value.Color, Capsule.Value.Radius, Capsule.Value.HalfHeight, Capsule.Value.NumOfSides,
					Capsule.Value.RenderOnTopOfEverything ? SDPG_Foreground : SDPG_World, Capsule.Value.Thickness);
			}
		}
	}

	virtual void DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override
	{
		if (const URCDebugShapeVisualizer* VizComponent = Cast<URCDebugShapeVisualizer>(Component))
		{
			for (const TPair<FName, FDebugStringData> StringData : VizComponent->DebugStrings)
			{
				FVector2D ScreenPos;
				if (!FSceneView::ProjectWorldToScreen(VizComponent->TransformLocation(StringData.Value.Location),
					View->UnconstrainedViewRect, View->ViewMatrices.GetViewProjectionMatrix(), ScreenPos))
					continue;

				ScreenPos /= Canvas->GetDPIScale(); // Dividing by DPI scale for monitors with a Display Scale set on a non-recommended value
				FSlateFontInfo FontInfo(GEngine->GetSmallFont(), (StringData.Value.FontScale > 0) ? StringData.Value.FontScale : 12, NAME_None);
				FString Text = StringData.Value.String.IsEmpty() ? StringData.Key.ToString() : StringData.Value.String;
				FCanvasTextItem TextItem(ScreenPos, FText::FromString(Text), FontInfo, StringData.Value.Color);
				StringData.Value.bDrawShadow ? TextItem.EnableShadow(FColor::Black) :TextItem.DisableShadow();

				Canvas->DrawItem(TextItem);
			}
		}
	}
};
