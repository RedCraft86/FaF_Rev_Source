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
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, MeshZoomRange.X))
	{
		if (MeshZoomRange.X < 0.1f) MeshZoomRange.X = 0.1f;
		if (MeshZoomRange.X > MeshZoomRange.Y)
		{
			MeshZoomRange.Y = MeshZoomRange.X + 0.1f;
		}

		return;
	}
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, MeshZoomRange.Y))
	{
		if (MeshZoomRange.Y < 0.2f) MeshZoomRange.Y = 0.2f;
		if (MeshZoomRange.Y < MeshZoomRange.X)
		{
			MeshZoomRange.X = MeshZoomRange.Y - 0.1f;
		}

		return;
	}

	for (FTransformMeshData& Data : MeshData)
	{
		Data.FillMaterials();
	}
}
#endif
