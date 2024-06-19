// Copyright (C) RedCraft86. All Rights Reserved.

#include "InventoryItemData.h"

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
	if (Priority == 0 && (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UInventoryItemData, ItemType)
		|| PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UInventoryItemData, Priority)))
	{
		switch (ItemType)
		{
		case EInventoryItemType::Basic: Priority = 5; break;
		case EInventoryItemType::Objective: Priority = 2; break;
		case EInventoryItemType::Consumable: Priority = 3; break;
		case EInventoryItemType::Equipment: Priority = 1; break;
		case EInventoryItemType::Viewable: Priority = 4; break;
		default: Priority = 6; break;
		}
	}
	if (bUpdate || PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UInventoryItemData, PreviewMesh))
	{
		bUpdate = false;
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

	if (ItemType == EInventoryItemType::Any)
	{
		ItemType = EInventoryItemType::Basic;
	}

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