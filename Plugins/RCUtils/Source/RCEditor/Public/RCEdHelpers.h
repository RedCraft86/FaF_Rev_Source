// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

namespace RCEdHelpers
{
	RCEDITOR_API FORCEINLINE TSharedRef<SWidget> CreatePropertyValueWidget(
		const TSharedPtr<IPropertyHandle>& PropertyHandle, const TAttribute<bool>& Enabled = true,
		const TAttribute<FText>& TooltipText = FText::FromString(TEXT("")))
	{
		TSharedRef<SWidget> Widget = PropertyHandle->CreatePropertyValueWidget();
		Widget->SetEnabled(Enabled);
		Widget->SetToolTipText(TooltipText);
		return Widget;
	};

	RCEDITOR_API FORCEINLINE void DrawArcWithThickness(FPrimitiveDrawInterface* PDI, const FVector& Base,
		const FVector& X, const FVector& Y, const float MinAngle, const float MaxAngle, const float Radius,
		const int Sections, const FLinearColor& Color, const uint8 DepthPriority, const float Thickness)
	{
		const float AngleStep = (MaxAngle - MinAngle) / Sections;
		float CurrentAngle = MinAngle;

		FVector LastVertex = Base + Radius * (FMath::Cos(CurrentAngle * (PI / 180.0f)) * X + FMath::Sin(CurrentAngle * (PI / 180.0f)) * Y);
		CurrentAngle += AngleStep;

		for (int32 i = 0; i < FMath::Max(1, Sections); i++)
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

	RCEDITOR_API FORCEINLINE void DrawCappedConeWithThickness(FPrimitiveDrawInterface* PDI, const FTransform& Transform,
		const float ConeLength, const float ConeAngle, const int32 ConeSides, const int32 ArcFrequency,
		const int32 CapSegments, const FLinearColor& Color, const uint8 DepthPriority, const float Thickness)
	{
		int32 _ConeSides = ConeSides;
		
		// The cap only works if there are an even number of verts generated so add another if needed 
		if ((_ConeSides & 0x1) != 0)
		{
			++_ConeSides;
		}

		TArray<FVector> Verts;
		DrawWireCone(PDI, Verts, Transform, ConeLength, ConeAngle, _ConeSides, Color, DepthPriority, Thickness);

		// Draw arcs
		const int32 ArcCount = Verts.Num() / 2;
		for ( int32 i = 0; i < ArcCount; i += ArcFrequency )
		{
			const FVector X = Transform.GetUnitAxis( EAxis::X );
			FVector Y = Verts[i] - Verts[ArcCount + i]; Y.Normalize();

			DrawArcWithThickness(PDI, Transform.GetTranslation(), X, Y, -ConeAngle, ConeAngle,
				ConeLength, CapSegments, Color, DepthPriority, Thickness);
		}
	}
}

