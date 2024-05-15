// Copyright (C) RedCraft86. All Rights Reserved.

#include "InventoryItemData.h"

#define ALT_MESH_KEY TEXT("AltMeshIndex")

FString UInventoryItemData::GetTypeString() const
{
	switch (ItemType)
	{
	case EInventoryItemType::Consumable: return TEXT("Consumable");
	case EInventoryItemType::Equipment: return TEXT("Equipment");
	case EInventoryItemType::Viewable: return TEXT("Viewable Item");
	default: return TEXT("Objective Item");
	}
}

FTransformMeshData UInventoryItemData::GetMeshData(const TMap<FName, FString>& InMetadata) const
{
	FTransformMeshData Data = MeshData.IsEmpty() ? FTransformMeshData() : MeshData[0];
	if (const FString& IndexValue = InMetadata.FindRef(ALT_MESH_KEY); !IndexValue.IsEmpty())
	{
		const int32 Idx = FCString::Atoi(*IndexValue);
		if (MeshData.IsValidIndex(Idx)) Data = MeshData[Idx];
	}

	return Data;
}

#if WITH_EDITOR
void UInventoryItemData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, InspectZoomRange))
	{
		if (InspectZoomRange.X < 0.1f) InspectZoomRange.X = 0.1f;
		if (InspectZoomRange.X > InspectZoomRange.Y)
		{
			InspectZoomRange.Y = InspectZoomRange.X + 0.1f;
		}
	}
#if WITH_EDITORONLY_DATA
	else if (bUpdate || PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, MeshData))
	{
		bUpdate = false;
		for (FTransformMeshData& Data : MeshData)
		{
			Data.FillMaterials();
		}
	}
#endif
}
#endif