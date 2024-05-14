// Copyright (C) RedCraft86. All Rights Reserved.

#include "InventoryItemData.h"

#if WITH_EDITOR
void UInventoryItemData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, InspectionZoomRange.X))
	{
		if (InspectionZoomRange.X < 0.1f) InspectionZoomRange.X = 0.1f;
		if (InspectionZoomRange.X > InspectionZoomRange.Y)
		{
			InspectionZoomRange.Y = InspectionZoomRange.X + 0.1f;
		}
	}
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, InspectionZoomRange.Y))
	{
		if (InspectionZoomRange.Y < 0.2f) InspectionZoomRange.Y = 0.2f;
		if (InspectionZoomRange.Y < InspectionZoomRange.X)
		{
			InspectionZoomRange.X = InspectionZoomRange.Y - 0.1f;
		}
	}
}
#endif
