// Copyright (C) RedCraft86. All Rights Reserved.

#include "InventoryItemData.h"

FString UInventoryItemData::GetTypeString() const
{
	switch (ItemType)
	{
	case EInventoryItemType::Objective: return TEXT("Objective");
	case EInventoryItemType::Consumable: return TEXT("Consumable");
	case EInventoryItemType::Equipment: return TEXT("Equipment");
	case EInventoryItemType::Viewable: return TEXT("Viewable");
	default: return TEXT("Basic");
	}
}

FTransformMeshData UInventoryItemData::GetMeshData(const TMap<FName, FString>& InMetadata) const
{
	const FTransformMeshData AltData = AltMeshes.FindRef(InMetadata.FindRef(NativeItemKeys::AltMeshID));
	if (AltData.IsValidData()) return AltData;
	return PreviewMesh;
}

#if WITH_EDITOR
void UInventoryItemData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
#if WITH_EDITORONLY_DATA
	if (bUpdate || PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UInventoryItemData, PreviewMesh))
	{
		PreviewMesh.FillMaterials();
	}
	if (bUpdate || PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UInventoryItemData, AltMeshes))
	{
		bUpdate = false;
		for (TPair<FString, FTransformMeshData>& Data : AltMeshes)
		{
			Data.Value.FillMaterials();
		}

		return;
	}
#endif

	for (const FName Key : NativeItemKeys::All)
	{
		if (!DefaultMetadata.Contains(Key)) DefaultMetadata.Add(Key);
	}
	
	if (PreviewZoomRange.X < 0.1f) PreviewZoomRange.X = 0.1f;
	if (PreviewZoomRange.X > PreviewZoomRange.Y)
	{
		PreviewZoomRange.Y = PreviewZoomRange.X + 0.1f;
	}
}
#endif